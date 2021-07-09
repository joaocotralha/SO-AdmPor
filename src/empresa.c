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
#include "memory.h"
#include "sotime.h"
#include "empresa.h"

extern struct statistics Ind;
extern struct configuration Config;

int empresa_executar(int id) {
	struct operation operacao;
	int clientes_servidos_por_empresas = 0;

	setbuf(stdout, NULL);

	// Preparar operacoes
	while (1) {
		// aguardar pedido de intermediario
		if (memory_request_r_read(id, &operacao) == 0)
			break;
		// preparar operacao e aguardar processamento de pedido
		time_processing_order();
		// colocar pedido
		memory_response_s_write(id, &operacao);
		Ind.agendamentos_entregues_por_empresas[operacao.id * Config.EMPRESAS + id]++;
		clientes_servidos_por_empresas++;
	}
	//    printf("EMPRESA %02d terminou\n",id);
	return clientes_servidos_por_empresas;
}
