#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "memory.h"
#include "prodcons.h"
#include "control.h"
#include "file.h"
#include "sotime.h"
#include "intermediario.h"
#include "cliente.h"
#include "empresa.h"


struct statistics Ind;     		// indicadores do funcionamento do SO_AdmPor
struct configuration Config; 	// configuração da execução do SO_AdmPor

/* main_cliente recebe como parâmetro o nº de clientes a criar */
void main_cliente(int quant)
{
    //CRIAR PROCESSOS
    //==============================================
    //
    // criar um número de processos cliente igual a quant através de um ciclo com i=0,1,...
  // após a criação de cada processo, o processo filho realiza duas atividades:
  // - chama a função cliente_executar passando como parâmetro o valor da variável de controlo do ciclo i=0,1,...
  // - chama a função exit passando como parâmetro o valor devolvido pela função cliente_executar
  // o processo pai guarda o pid do filho no vetor Ind.pid_clientes[i], com i=0,1,... e termina normalmente a função
    //

    for(int i = 0; i < quant; i++) {
      int pid = fork();

      if(pid == -1) {
        perror("Erro na criação de cliente");
        exit(1);
      }

      if(pid == 0) {
        int res = cliente_executar(i);
        exit(res);
      } else {
        Ind.pid_clientes[i] = pid;
      }
    }
    
    //
    //so_main_cliente(quant);
    //==============================================
}

/* main_intermediario recebe como parâmetro o nº de intermediarios a criar */
void main_intermediario(int quant)
{

    // CRIAR PROCESSOS
    //==============================================
    for(int i = 0; i < quant; i++) {
      int pid = fork();

      if(pid == -1) {
        perror("Erro na criação de intermediário");
        exit(1);
      }

      if(pid == 0) {
        int res = intermediario_executar(i);
        exit(res);
      } else {
        Ind.pid_intermediarios[i] = pid;
      }
    }
    //so_main_intermediario(quant);
    //==============================================
}

/* main_empresas recebe como parâmetro o nº de empresas a criar */
void main_empresas(int quant)
{

    // CRIAR PROCESSOS
    //==============================================
    for(int i = 0; i < quant; i++) {
      int pid = fork();

      if(pid == -1) {
        perror("Erro na criação de empresa");
        exit(1);
      }

      if(pid == 0) {
        int res = empresa_executar(i);
        exit(res);
      } else {
        Ind.pid_empresas[i] = pid;
      }
    }
    //so_main_empresas(quant);
    //==============================================
}

int main(int argc, char *argv[])
{
    char *ficEntrada = NULL;
    char *ficSaida = NULL;
    char *ficLog = NULL;
    long intervalo = 0;

    // TRATAR PARÂMETROS DE ENTRADA
    //==============================================
    if(argc < 2) {
      printf("\nUtilização: soadmpor file_configuracao\n\n");
      printf("Opções: [file_resultados]\n-l [file_log]\n-t [intervalo(us)]\n\n");
      exit(1);
    }

    ficEntrada = argv[1];

    for(int i = 2; i < argc; i++) {

      char *param = argv[i];
      if((strcmp(param, "-l") == 0) && (ficLog == NULL)) {
        ficLog = argv[i+1];
        i++;
      }
      else
      if((strcmp(param, "-t") == 0) && (intervalo == 0)) {
        intervalo = (long) argv[i+1];
        i++;
      }
      else
      if(ficSaida == NULL) {
        ficSaida = param;
      }

    }
    //intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

    printf(
	"\n------------------------------------------------------------------------");
    printf(
	"\n----------------------------- SO_AdmPor ------------------------------");
    printf(
	"\n------------------------------------------------------------------------\n");

    // Ler dados de entrada
    file_begin(ficEntrada, ficSaida, ficLog);
    // criar zonas de memória e semáforos
    memory_create_buffers();
    prodcons_create_buffers();
    control_create();

    // Criar estruturas para indicadores e guardar capacidade inicial de portuaria
    memory_create_statistics();

    printf("\n\t\t\t*** Open SO_AdmPor ***\n\n");
    control_open_soadmpor();

    // Registar início de operação e armar alarme
    time_begin(intervalo);

    //
    // Iniciar sistema
    //

    // Criar INTERMEDIARIOS
    main_intermediario(Config.INTERMEDIARIO);
    // Criar EMPRESAS
    main_empresas(Config.EMPRESAS);
    // Criar CLIENTES
    main_cliente(Config.CLIENTES);

    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR ESTATISTICAS
    //==============================================
    for(int i = 0; i < Config.CLIENTES; i++) {

      int status;
      waitpid(Ind.pid_clientes[i], &status, 0);
      if(WIFEXITED(status)) {

        int exitstatus = WEXITSTATUS(status);
        if(exitstatus < Config.OPERATIONS) {
          Ind.servicos_recebidos_por_clientes[exitstatus]++;
        }
      }
    }
    //so_main_wait_clientes();
    //==============================================

    // Desarmar alarme
    time_destroy(intervalo);

    printf("\n\t\t\t*** Close SO_AdmPor ***\n\n");
    control_close_soadmpor();

    // ESPERAR PELA TERMINAÇÃO DOS INTERMEDIARIOS E ATUALIZAR INDICADORES
    //==============================================
    for(int i = 0; i < Config.INTERMEDIARIO; i++) {

      int status;
      waitpid(Ind.pid_intermediarios[i], &status, 0);
      if(WIFEXITED(status)) {

        int exitstatus = WEXITSTATUS(status);
        Ind.clientes_servidos_por_intermediarios[i] = exitstatus;
      }
    }
    //so_main_wait_intermediarios();
    //==============================================

    // ESPERAR PELA TERMINAÇÃO DAS EMPRESAS E ATUALIZAR INDICADORES
    //==============================================
    for(int i = 0; i < Config.EMPRESAS; i++) {

      int status;
      waitpid(Ind.pid_empresas[i], &status, 0);
      if(WIFEXITED(status)) {

        int exitstatus = WEXITSTATUS(status);
        Ind.clientes_servidos_por_empresas[i] = exitstatus;
      }
    }
    //so_main_wait_empresas();
    //==============================================

    printf(
  "------------------------------------------------------------------------\n\n");
    printf("\t\t\t*** Statistics ***\n\n");
    so_write_statistics();
    printf("\t\t\t*******************\n");

    // destruir zonas de memória e semáforos
    file_destroy();
    control_destroy();
    prodcons_destroy();
    //memory_destroy_all();

    return 0;
}
