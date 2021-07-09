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
#include "intermediario.h"

int intermediario_executar(int id) {
	int ret;
	struct operation operacao;
	int clientes_servidos_por_intermediarios = 0;

	setbuf(stdout, NULL);

	// Passar descricao de operação para a empresa
	while (1) {
		ret = memory_request_d_read(id, &operacao);
		if (ret == 1) {
			// operacao disponível
			memory_request_r_write(id, &operacao);
		} else if (ret == 2) {
			// operacao indisponível
			memory_response_s_write(id, &operacao);
		} else
			// sem clientes
			break;
		clientes_servidos_por_intermediarios++;
	}

	return clientes_servidos_por_intermediarios;
}
