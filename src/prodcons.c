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
#include "prodcons.h"
#include "control.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct prodcons ProdCons;

//******************************************
// SEMAFORO_CRIAR
//
sem_t * semaphore_create(char * name, int value) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
	//

	sem_t *sem_ptr;

	sem_unlink(name);
	sem_ptr = sem_open(name, O_CREAT | O_EXCL, 0666, value);
	if(sem_ptr == SEM_FAILED) {
		perror(name);
		exit(1);
	}
	return sem_ptr;

	//
	//return so_semaphore_create(name, value);
	//==============================================
}

void prodcons_create_capacidade_portuaria() {
	//==============================================
	// CRIAR MUTEX PARA CONTROLAR O ACESSO A CAPACIDADE_PORTUARIA
	//
	// utilizar a função genérica semaphore_create
	//

	ProdCons.capacidade_portuaria_mutex = semaphore_create("sem_cap_port", 1);

	//
	//so_prodcons_create_capacidade_portuaria();
	//==============================================
}

void prodcons_create_buffers() {
	//==============================================
	// CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
	//
	// utilizar a função genérica semaphore_create
	//

	// semáforos de controlo do acesso ao buffer entre empresas e clientes
	ProdCons.response_s_full = semaphore_create("sem_s_full", 1);
	ProdCons.response_s_empty = semaphore_create("sem_s_empty", 1);
	ProdCons.response_s_mutex = semaphore_create("sem_s_mutex", 1);

	// semáforos de controlo do acesso ao buffer entre intermediarios e empresas
	ProdCons.request_r_full = semaphore_create("sem_r_full", 1);
	ProdCons.request_r_empty = semaphore_create("sem_r_empty", 1);
	ProdCons.request_r_mutex = semaphore_create("sem_r_mutex", 1);

	// semáforos de controlo do acesso ao buffer entre clientes e intermediarios
	ProdCons.request_d_full = semaphore_create("sem_d_full", 1);
	ProdCons.request_d_empty = semaphore_create("sem_d_empty", 1);
	ProdCons.request_d_mutex = semaphore_create("sem_d_mutex", 1);

	//
	//so_prodcons_create_buffers();
	//==============================================
}

void semaphore_destroy(char * name, void * ptr) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO
	//

	if(sem_close(ptr) == -1) {
		perror(name);
	}
	if(sem_unlink(name) == -1) {
		perror(name);
	}

	//
	//so_semaphore_destroy(name, ptr);
	//==============================================
}

void prodcons_destroy() {
	//==============================================
	// DESTRUIR SEMÁFORO E RESPETIVO NOME
	//
	// utilizar a função genérica semaphore_destroy
	//

	// semaforo para exclusao mutua no acesso a capacidade portuaria
	semaphore_destroy("sem_cap_port", ProdCons.capacidade_portuaria_mutex);

	// semáforos de controlo do acesso ao buffer entre empresas e clientes
	semaphore_destroy("sem_s_full", ProdCons.response_s_full);
	semaphore_destroy("sem_s_empty", ProdCons.response_s_empty);
	semaphore_destroy("sem_s_mutex", ProdCons.response_s_mutex);

	// semáforos de controlo do acesso ao buffer entre intermediarios e empresas
	semaphore_destroy("sem_r_full", ProdCons.request_r_full);
	semaphore_destroy("sem_r_empty", ProdCons.request_r_empty);
	semaphore_destroy("sem_r_mutex", ProdCons.request_r_mutex);

	// semáforos de controlo do acesso ao buffer entre clientes e intermediarios
	semaphore_destroy("sem_d_full", ProdCons.request_d_full);
	semaphore_destroy("sem_d_empty", ProdCons.request_d_empty);
	semaphore_destroy("sem_d_mutex", ProdCons.request_d_mutex);

	//
	//so_prodcons_destroy();
	//==============================================
}

//******************************************
void prodcons_request_d_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER DESCRICAO
	//
	if(sem_wait(ProdCons.request_d_empty) == -1) {
		perror("sem wait produce request_d");
	}
	//
	//so_prodcons_request_d_produce_begin();
	//==============================================
}

//******************************************
void prodcons_request_d_produce_end() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER DESCRICAO
	//
	if(sem_post(ProdCons.request_d_full) == -1) {
		perror("sem post produce request_d");
	}
	//
	//so_prodcons_request_d_produce_end();
	//==============================================
}

//******************************************
void prodcons_request_d_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER DESCRICAO
	//
	if(sem_wait(ProdCons.request_d_full) == -1) {
		perror("sem wait consume request_d");
	}
	//
	//so_prodcons_request_d_consume_begin();
	//==============================================
}

//******************************************
void prodcons_request_d_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER DESCRICAO
	//
	if(sem_post(ProdCons.request_d_empty) == -1) {
		perror("sem post consume request_d");
	}
	//
	//so_prodcons_request_d_consume_end();
	//==============================================
}

//******************************************
void prodcons_request_r_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER PEDIDO
	//
	if(sem_wait(ProdCons.request_r_empty) == -1) {
		perror("sem wait produce request_r");
	}
	//
	//so_prodcons_request_r_produce_begin();
	//==============================================
}

//******************************************
void prodcons_request_r_produce_end() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER PEDIDO
	//
	if(sem_post(ProdCons.request_r_full) == -1) {
		perror("sem post produce request_r");
	}
	//
	//so_prodcons_request_r_produce_end();
	//==============================================
}

//******************************************
void prodcons_request_r_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO
	//
	if(sem_wait(ProdCons.request_r_full) == -1) {
		perror("sem wait consume request_r");
	}
	//
	//so_prodcons_request_r_consume_begin();
	//==============================================
}

//******************************************
void prodcons_request_r_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO
	//
	if(sem_post(ProdCons.request_r_empty) == -1) {
		perror("sem post consume request_r");
	}
	//
	//so_prodcons_request_r_consume_end();
	//==============================================
}

//******************************************
void prodcons_response_s_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER AGENDAMENTO
	//
	if(sem_wait(ProdCons.response_s_empty) == -1) {
		perror("sem wait produce response_s");
	}
	//
	//so_prodcons_response_s_produce_begin();
	//==============================================
}

//******************************************
void prodcons_response_s_produce_end() {
	//==============================================
	// CONTROLAR ACESSO AO BUFFER AGENDAMENTO
	//
	if(sem_post(ProdCons.response_s_full) == -1) {
		perror("sem post produce response_s");
	}
	//
	//so_prodcons_response_s_produce_end();
	//==============================================
}

//******************************************
void prodcons_response_s_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER AGENDAMENTO
	//
	if(sem_wait(ProdCons.response_s_full) == -1) {
		perror("sem wait consume response_s");
	}
	//
	//so_prodcons_response_s_consume_begin();
	//==============================================
}

//******************************************
void prodcons_response_s_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER AGENDAMENTO
	//
	if(sem_post(ProdCons.response_s_empty) == -1) {
		perror("sem post consume response_s");
	}
	//
	//so_prodcons_response_s_consume_end();
	//==============================================
}

//******************************************
void prodcons_buffers_begin() {
	//==============================================
	// GARANTIR EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	if(sem_wait(ProdCons.response_s_mutex) == -1) {
		perror("sem wait buffer response_s");
	}
	if(sem_wait(ProdCons.request_r_mutex) == -1) {
		perror("sem wait buffer request_r");
	}
	if(sem_wait(ProdCons.request_d_mutex) == -1) {
		perror("sem wait buffer request_d");
	}
	//
	//so_prodcons_buffers_begin();
	//==============================================
}

//******************************************
void prodcons_buffers_end() {
	//==============================================
	// FIM DA ZONA DE EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	if(sem_post(ProdCons.response_s_mutex) == -1) {
		perror("sem post buffer response_s");
	}
	if(sem_post(ProdCons.request_r_mutex) == -1) {
		perror("sem post buffer request_r");
	}
	if(sem_post(ProdCons.request_d_mutex) == -1) {
		perror("sem post buffer request_d");
	}
	//
	//so_prodcons_buffers_end();
	//==============================================
}

//******************************************
int prodcons_update_capacidade_portuaria(int operacao) {
	//==============================================
	// OBTER MUTEX DA CAPACIDADE PORTUARIA E ATUALIZAR CAPACIDADE PORTUARIA
	//
	// se capacidade_portuaria da operacao > 0 então decrementá-lo de uma unidade e
	//   função devolve 1
	// se capacidade_portuaria da operacao = 0 então função devolve 0
	//
	int res = 0;

	if(sem_wait(ProdCons.capacidade_portuaria_mutex) == -1) {
		perror("sem wait mutex capacidade_portuaria");
	}

	if(Config.capacidade_portuaria[operacao] > 0) {
		Config.capacidade_portuaria[operacao]--;
		res++;
	}

	if(sem_post(ProdCons.capacidade_portuaria_mutex) == -1) {
		perror("sem post mutex capacidade_portuaria");
	}

	return res;
	//
	//return so_prodcons_update_capacidade_portuaria(operacao);
	//==============================================
}
