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
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "memory.h"
#include "prodcons.h"
#include "file.h"
#include "sotime.h"
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct request_d BDescricao; 	// buffer cliente-intermediario (request_description)
extern struct request_r BPedido; 	// buffer intermediario-empresa (request_operation)
extern struct response_s BAgendamento;  	// buffer empresa-cliente (response_scheduling)
extern struct configuration Config;
//==============================================

struct file Ficheiros; // informação sobre nomes e handles de ficheiros

void file_begin(char *fic_in, char *fic_out, char *fic_log) {
	//==============================================
	// GUARDAR NOMES DOS FICHEIROS NA ESTRUTURA Ficheiros
	//
	Ficheiros.entrada = malloc(strlen(fic_in));
	Ficheiros.saida = malloc(strlen(fic_out));
	Ficheiros.log = malloc(strlen(fic_log));

	strcpy(Ficheiros.entrada, fic_in);
	strcpy(Ficheiros.saida, fic_out);
	strcpy(Ficheiros.log, fic_log);
	//
	//so_file_begin(fic_in, fic_out, fic_log);
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE ENTRADA
	// em modo de leitura
	//
	Ficheiros.h_in = fopen(Ficheiros.entrada, "r");
	//
	//so_file_open_file_in();
	//==============================================

	// parse do ficheiro de teste
	// esta funcao prepara os campos da estrutura Config (char *)
	// com os dados do ficheiro de entrada
	//
	if (ini_parse_file(Ficheiros.h_in, handler, &Config) < 0) {
		printf("Erro a carregar o ficheiro de teste'\n");
		exit(1);
	}
	//
	// agora e' preciso inicializar os restantes campos da estrutura Config
	//
	//==============================================
	// CONTAR OPERACOES
	// usar strtok para percorrer Config.list_operacoes
	// guardar resultado em Config.OPERACOES
	//
	int count = 0;
	char *dup = strdup(Config.list_operacoes);
	char *token = strtok(dup, " ");

	while(token != NULL) {
		token = strtok(NULL, " ");
		count++;
	}

	Config.OPERATIONS = count;
	//
	//so_file_count_operacoes();
	//==============================================

	// iniciar memoria para o vetor com o capacidade portuaria e semaforo
	memory_create_capacidade_portuaria();
	prodcons_create_capacidade_portuaria();

	//==============================================
	// LER CAPACIDADE DE CADA EMPRESA
	// percorrer Config.list_operacoes e
	// guardar cada valor no vetor Config.capacidade_portuaria
	//
	count = 0;
	dup = strdup(Config.list_operacoes);
	token = strtok(dup, " ");

	while(token != NULL) {
		Config.capacidade_portuaria[count] = atoi(token);
		token = strtok(NULL, " ");
		count++;
	}
	//
	//so_file_read_capacidade_portuaria();
	//==============================================

	//==============================================
	// LER NR DE OPERACOES DE CADA CLIENTE
	// guardar valor de Config.nr_operacoes em Config.N
	//
	Config.N = atoi(Config.nr_operacoes);
	//
	//so_file_read_numero_operacoes();
	//==============================================

	//==============================================
	// CONTAR CLIENTES
	// usar strtok para percorrer Config.list_clientes
	// guardar resultado em Config.CLIENTES
	//
	//a utilizacao de um duplicado resulta no funcionamento incorreto
	//da funcao so_file_read_operacoes() chamada mais a' frente
	count = 0;
	token = strtok(Config.list_clientes, " ");

	while(token != NULL) {
		token = strtok(NULL, " ");
		count++;
	}

	Config.CLIENTES = count;
	//
	//so_file_count_clientes();
	//==============================================

	//==============================================
	// CONTAR INTERMEDIARIOS
	// usar strtok para percorrer Config.list_intermediarios
	// guardar resultado em Config.INTERMEDIARIOS
	//
	count = 0;
	dup = strdup(Config.list_intermediarios);
	token = strtok(dup, " ");

	while(token != NULL) {
		token = strtok(NULL, " ");
		count++;
	}

	Config.INTERMEDIARIO = count;
	//
	//so_file_count_intermediarios();
	//==============================================

	//==============================================
	// CONTAR EMPRESAS
	// usar strtok para percorrer Config.list_empresas
	// guardar resultado em Config.EMPRESAS
	//
	count = 0;
	dup = strdup(Config.list_empresas);
	token = strtok(dup, ",");

	while(token != NULL) {
		token = strtok(NULL, ",");
		count++;
	}

	Config.EMPRESAS = count;
	//
	//so_file_count_empresas();
	//==============================================

	so_file_read_operacoes();

	//==============================================
	// LER CAPACIDADES DOS BUFFERS
	// usar strtok para percorrer Config.list_buffers
	// guardar primeiro tamanho em Config.BUFFER_DESCRICAO
	// guardar segundo tamanho em Config.BUFFER_PEDIDO
	// guardar terceiro tamanho em Config.BUFFER_AGENDAMENTO
	//
	dup = strdup(Config.list_buffers);
	token = strtok(dup, " ");
	Config.BUFFER_DESCRICAO = atoi(token);
	token = strtok(dup, " ");
	Config.BUFFER_PEDIDO = atoi(token);
	token = strtok(dup, " ");
	Config.BUFFER_AGENDAMENTO = atoi(token);
	//
	//so_file_read_capacidade_buffer();
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE SAIDA (se foi especificado)
	// em modo de escrita
	//
	if(Ficheiros.saida != NULL) {
		Ficheiros.h_out = fopen(Ficheiros.saida, "w");
	}
	//
	//so_file_open_file_out();
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE LOG (se foi especificado)
	// em modo de escrita
	//
	if(Ficheiros.log != NULL) {
		Ficheiros.h_log = fopen(Ficheiros.log, "w");
	}
	//
	//so_file_open_file_log();
	//==============================================
}

void file_destroy() {
	//==============================================
	// LIBERTAR ZONAS DE MEMÓRIA RESERVADAS DINAMICAMENTE
	// que podem ser: Ficheiros.entrada, Ficheiros.saida, Ficheiros.log
	//
	fclose(Ficheiros.h_in);
	fclose(Ficheiros.h_out);
	fclose(Ficheiros.h_log);

	free(Ficheiros.entrada);
	free(Ficheiros.saida);
	free(Ficheiros.log);
	//
	//so_file_destroy();
	//==============================================
}

void file_write_log_file(int etapa, int id) {
	double t_diff;

	if (Ficheiros.h_log != NULL) {

		prodcons_buffers_begin();

		// guardar timestamp
		t_diff = time_untilnow();

		//==============================================
		// ESCREVER DADOS NO FICHEIRO DE LOG
		//
		// o log deve seguir escrupulosamente o formato definido
		//
		fprintf(Ficheiros.h_log,"tempo = %f\t", t_diff);
		fprintf(Ficheiros.h_log,"etapa = %d\t", etapa);
		fprintf(Ficheiros.h_log,"id = %d", id);

		int size = Config.BUFFER_DESCRICAO;
		fputs("\nB_DESCRICAO", Ficheiros.h_log);
		fputs("\tOPERACAO\tcliente\tintermediario\tempresa\n", Ficheiros.h_log);
		for(int i = 0; i < size; i++) {
			fwrite(BDescricao.buffer, size, sizeof(int), Ficheiros.h_log);
		}
		fputs("\nB_PEDIDO", Ficheiros.h_log);
		fputs("\tOPERACAO\tcliente\tintermediario\tempresa\n", Ficheiros.h_log);
		for(int i = 0; i < size; i++) {
			fwrite(BPedido.buffer, size, sizeof(int), Ficheiros.h_log);
		}
		fputs("\nB_DESCRICAO", Ficheiros.h_log);
		fputs("\tOPERACAO\tcliente\tintermediario\tempresa\n", Ficheiros.h_log);
		for(int i = 0; i < size; i++) {
			fwrite(BAgendamento.buffer, size, sizeof(int), Ficheiros.h_log);
		}
		//
		//so_file_write_log_file(etapa, id, t_diff);
		//==============================================

		prodcons_buffers_end();
	}
}

void file_write_line(char * linha) {
	//==============================================
	// ESCREVER UMA LINHA NO FICHEIRO DE SAIDA
	//
	fputs(linha, Ficheiros.h_out);
	//
	//so_file_write_line(linha);
	//==============================================
}

int stricmp(const char *s1, const char *s2) {
	if (s1 == NULL)
		return s2 == NULL ? 0 : -(*s2);
	if (s2 == NULL)
		return *s1;

	char c1, c2;
	while ((c1 = tolower(*s1)) == (c2 = tolower(*s2))) {
		if (*s1 == '\0')
			break;
		++s1;
		++s2;
	}

	return c1 - c2;
}

int handler(void* user, const char* section, const char* name,
		const char* value) {
	struct configuration* pconfig = (struct configuration*) user;

#define MATCH(s, n) stricmp(section, s) == 0 && stricmp(name, n) == 0
	if (MATCH("operacoes", "capacidade_portuaria")) {
		pconfig->list_operacoes = strdup(value);
	} else if (MATCH("clientes", "operacao")) {
		pconfig->list_clientes = strdup(value);
	} else if (MATCH("clientes", "N")) {
		pconfig->nr_operacoes = strdup(value);
	} else if (MATCH("intermediarios", "list")) {
		pconfig->list_intermediarios = strdup(value);
	} else if (MATCH("empresas", "operacoes")) {
		pconfig->list_empresas = strdup(value);
	} else if (MATCH("buffers", "capacidade_buffer")) {
		pconfig->list_buffers = strdup(value);
	} else {
		return 0; /* unknown section/name, error */
	}
	return 1;
}
