#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct scheduler Schedule; // estrutura de suporte às decisões de scheduling

void scheduler_begin(int empresa, int operacao) {
	//==============================================
	// REGISTAR QUE A EMPRESA REALIZA ESTA OPERACAO
	//
	// escrever 1 na posição respetiva do vetor Schedule.ptr
	so_scheduler_begin(empresa, operacao);
	//==============================================
}

int scheduler_get_empresa(int intermediario, int operacao) {
	//==============================================
	// ESCOLHER EMPRESA PARA SOLICITAR OPERACAO
	//
	// dada uma operacao, escolher uma das empresas que realiza a operacao
	//
	// a informação do intermdiario que está a pedir uma empresa
	// pode não ser necessária
	return so_scheduler_get_empresas(intermediario, operacao);
	//==============================================
}
