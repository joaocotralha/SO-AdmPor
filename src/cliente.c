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

#include "cliente.h"
#include "main.h"
#include "memory.h"
#include "file.h"
#include "sotime.h"
#include "so.h"

extern struct statistics Ind;
extern struct configuration Config;

int cliente_executar(int id) {

	//==============================================
	// EXECUTAR OPERACOES DO CLIENTE
	//
	// modificar cliente do Projeto 1:
	// - buf deve ter capacidade para pelo menos 500 chars
	// - repetir a mesma operacao Config.N vezes (escrever
	// e ler de memória, processar e imprimir resultados)
	// - fazer sleep de 1ms entre pedidos
	// - guardar operacao.cliente e tempo de execucao de cada
	// operacao em Ind.tempos, usando id, Config.N e i para indexar
	// - modificar mensagem impressa para ficar igual à de
	// soadmpor e para usar dados vindos de operacao
	//
	int n, count, ret;
	struct operation operacao;
	int operation_id;
	char buf[500 * sizeof(char)];
	char *result;

	setbuf(stdout, NULL);

	n = 0;
	count = 0;
	result = Config.list_clientes;
	while (n < id) {
		while (Config.list_clientes[count++] != '\0')
			;
		result = &Config.list_clientes[count];
		n++;
	}

	operation_id = atoi(result);
	operacao.id = operation_id;
	operacao.cliente = id;

	for(int i = 0; i < Config.N; i++) {

		memory_request_d_write(id, &operacao);
		memory_response_s_read(id, &operacao);

		if (operacao.disponibilidade == 1) {
			printf(
					"     CLIENTE %03d solicitou %d e obteve %d (v:%02d c:%02d t:%.9fs)\n",
					id, operation_id, operacao.id, operacao.intermediario, operacao.empresa,
					time_difference(operacao.time_descricao, operacao.time_agendamento));
			sprintf(buf, "     CLIENTE %03d solicitou %d e obteve %d\n", id,
					operation_id, operacao.id);
			ret = operacao.id;
		} else {
			printf("     CLIENTE %03d solicitou %d mas nao estava disponivel!\n",
					id, operation_id);
			sprintf(buf,
					"     CLIENTE %03d solicitou %d mas nao estava disponivel!\n",
					id, operation_id);
			ret = Config.OPERATIONS;
		}

		file_write_line(buf);

		Ind.tempos[Config.N * id + i].cliente = operacao.cliente;
		Ind.tempos[Config.N * id + i].time = time_difference(operacao.time_descricao, operacao.time_agendamento);

		usleep (1000);
	}

	return ret;
	//
	//return so_cliente_operations(id);
	//==============================================
}
