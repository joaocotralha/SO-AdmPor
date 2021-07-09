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

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct control *Controlo;          // controlo da abertura do SO_AdmPor

void cond_create(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UMA CONDIÇÃO PARTILHÁVEL ENTRE PROCESSOS
	//
	so_cond_create(cond, mutex);
	//==============================================
}

void control_create() {
	//==============================================
	// CRIAÇÃO DE ZONA DE MEMÓRIA PARTILHADA
	//
	// Criar e iniciar variaveis de condicao e respetivos mutexes do Controlo
	so_control_create();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS DE CONTROLO DE ABERTURA DO SO_ADMPOR
	//
	// Criar e iniciar variaveis de condicao e respetivos mutexes do Controlo
	so_control_create_opening();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS PARA EMPRESA ACORDAR CLIENTE A QUE SE DESTINA O AGENDAMENTO
	//
	// Criar e iniciar semáforos
	so_control_create_wakeup_cliente();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS PARA INTERMEDIARIO ACORDAR EMPRESA A QUE SE DESTINA O PEDIDO DE OPERACAO
	//
	// Criar e iniciar semáforos
	so_control_create_wakeup_empresa();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS PARA CLIENTE ACORDAR INTERMEDIARIO A QUE SE DESTINA A DESCRICAO
	//
	// Criar e iniciar semáforos
	so_control_create_wakeup_intermediario();
	//==============================================

}

void cond_destroy(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE VARIÁVEIS DE CONDIÇÃO
	//
	so_cond_destroy(cond, mutex);
	//==============================================
}

void control_destroy() {
	//==============================================
	// DESTRUIÇÃO DE MECANISMOS DE CONTROLO DE ABERTURA DO SO_ADMPOR
	//
	// utilizar a função genérica cond_destroy
	so_control_destroy_opening();
	//==============================================

	//==============================================
	// DESTRUIR SEMÁFOROS E RESPETIVOS NOMES
	//
	// utilizar a função genérica prodcons_destroy
	so_control_destroy_wakeup_cliente();
	so_control_destroy_wakeup_empresa();
	so_control_destroy_wakeup_intermediario();
	//==============================================

	//==============================================
	// DESTRUIR ZONA DE MEMÓRIA
	//
	// utilizar a função genérica memory_destroy
	so_control_destroy();
	//==============================================
}

//***************************************************
// ABERTURA DO SO_ADMPOR
//
void control_open_soadmpor() {
	//==============================================
	// ABRIR SO_ADMPOR
	//
	so_control_open_soadmpor();
	//==============================================
}

void control_close_soadmpor() {
	//==============================================
	// FECHAR SO_ADMPOR
	//
	so_control_close_soadmpor();
	//==============================================
}

//***************************************************
// CONTROLO ENTRE CLIENTE/INTERMEDIARIO/EMPRESA
//
void control_cliente_submete_descricao(int id) {
	//==============================================
	// SINALIZAR INTERMEDIARIO DA SUBMISSÃO DE DESCRICAO
	//
	so_control_cliente_submete_descricao(id);
	//==============================================
}
//******************************************
int control_intermediario_esperapor_descricao(int id) {
	//==============================================
	// INTERMEDIARIO AGUARDA SUBMISSÃO DE DESCRICAO
	//
	// verificar também se SO_AdmPor está open
	return so_control_intermediario_esperapor_descricao(id);
	//==============================================
}
//******************************************
void control_intermediario_submete_pedido(int id) {
	//==============================================
	// SINALIZAR EMPRESA DA SUBMISSÃO DE PEDIDO DE OPERACAO
	//
	so_control_intermediario_submete_pedido(id);
	//==============================================
}
//******************************************
int control_empresa_esperapor_pedido(int id) {
	//==============================================
	// EMPRESA AGUARDA SUBMISSÃO DE PEDIDO DE OPERACAO
	//
	// verificar também se SO_AdmPor está open
	return so_control_empresa_esperapor_pedido(id);
	//==============================================
}
//******************************************
void control_empresa_submete_agendamento(int id) {
	//==============================================
	// SINALIZAR CLIENTE DA SUBMISSÃO DE AGENDAMENTO
	//
	so_control_empresa_submete_agendamento(id);
	//==============================================
}
//******************************************
void control_cliente_esperapor_agendamento(int id) {
	//==============================================
	// CLIENTE AGUARDA SUBMISSÃO DE AGENDAMENTO
	//
	so_control_cliente_esperapor_agendamento(id);
	//==============================================
}
