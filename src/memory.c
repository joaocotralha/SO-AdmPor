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
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct pointers Pointer;
extern struct operation Operation;
extern struct scheduler Schedule;
extern struct statistics Ind;
extern struct configuration Config;
//==============================================

struct request_d BDescricao; 	// buffer cliente-intermediario (request_description)
struct request_r BPedido; 	// buffer intermediario-empresa (request_operation)
struct response_s BAgendamento;  	// buffer empresa-cliente (response_scheduling)

//******************************************
// CRIAR ZONAS DE MEMORIA
//
void * memory_create(char * name, int size) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
	//
	// usar getuid() para gerar nome unico na forma:
	// sprintf(name_uid,"/%s_%d", name, getuid())
	// usar name_uid em shm_open
	// usar tambem: ftruncate e mmap
	//

	char name_uid[42];
	int ret;
	int *ptr;

	sprintf(name_uid, "/%s_%d", name, getuid());

	int shm_seg = shm_open(name_uid, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if(shm_seg == -1) {
		perror("\nErro na criacao de memoria partilhada\n");
		exit(1);
	}

	ret = ftruncate(shm_seg, size);
	if(ret == -1) {
		perror("\nErro na definicao de tamanho do espaco da memoria partilhada\n");
		exit(2);
	}

	ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_seg, 0);
	if(ptr == MAP_FAILED) {
		perror("\nErro na criacao de mapa da memoria\n");
		exit(3);
	}

	return ptr;

	//
	//return so_memory_create(name, size);
	//==============================================
}
void memory_create_capacidade_portuaria() {
	//==============================================
	// CRIAR ZONA DE MEMÓRIA PARA A CAPACIDADE DE PORTUARIA
	//
	// utilizar a função genérica memory_create(char *,int)
	// para criar ponteiro que se guarda em Config.capacidade_portuaria
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [OPERACOES] para inteiro
	//

	int buff_size = Config.OPERATIONS * sizeof(int);

	Config.capacidade_portuaria = memory_create("shm_capacidade_portuaria",
																																		buff_size);
	//
	//so_memory_create_capacidade_portuaria();
	//==============================================
}
void memory_create_buffers() {
	//==============================================
	// CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS: DESCRICAO, PEDIDO e AGENDAMENTO
	//
	// utilizar a função genérica memory_create(char *,int)
	//
	// para criar ponteiro que se guarda em BAgendamento.ptr
	// que deve ter capacidade para struct pointers
	// para criar ponteiro que se guarda em BAgendamento.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_AGENDAMENTO] para struct operation
	//

	int ptr_size;
	int buff_size;

	ptr_size  = sizeof(Pointer);
	buff_size = Config.OPERATIONS * sizeof(int);

	BAgendamento.ptr    = memory_create("shm_agendamento_ptr", ptr_size);
	BAgendamento.buffer = memory_create("shm_agendamento_buffer", buff_size);

	//
	// para criar ponteiro que se guarda em BPedido.ptr
	// que deve ter capacidade para struct pointers
	// para criar ponteiro que se guarda em BPedido.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_PEDIDO] para struct operation
	//

	buff_size = Config.BUFFER_PEDIDO * sizeof(Operation);

	BPedido.ptr    = memory_create("shm_pedido_ptr", ptr_size);
	BPedido.buffer = memory_create("shm_pedido_buffer", buff_size);

	//
	// para criar ponteiro que se guarda em BDescricao.ptr
	// que deve ter capacidade para um vetor unidimensional
	// de inteiros com a dimensao [BUFFER_DESCRICAO]
	// para criar ponteiro que se guarda em BDescricao.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_DESCRICAO] para struct operation
	//

	ptr_size  = Config.BUFFER_DESCRICAO * sizeof(int);

	BDescricao.ptr    = memory_create("shm_descricao_ptr", ptr_size);
	BDescricao.buffer = memory_create("shm_descricao_buffer", buff_size);

	//
	//so_memory_create_buffers();
	//==============================================
}
void memory_create_scheduler() {
	//==============================================
	// CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
	//
	// utilizar a função genérica memory_create(char *,int)
	// para criar ponteiro que se guarda em Schedule.ptr
	// que deve ter capacidade para um vetor bidimensional
	// com a dimensao [OPERACOES,EMPRESAS] para inteiro
	//

	int buff_size = Config.OPERATIONS * Config.EMPRESAS * sizeof(int);

	Schedule.ptr = memory_create("shm_scheduler", buff_size);

	//
	//so_memory_create_scheduler();
	//==============================================
}

void memory_destroy(char * name, void * ptr, int size) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
	//
	// usar getuid() para gerar nome unico na forma:
	// sprintf(name_uid,"/%s_%d", name, getuid())
	// usar name_uid em shm_unlink
	// usar tambem: munmap
	//

	char name_uid[42];
	sprintf(name_uid, "/%s_%d", name, getuid());

	int ret_unlink = shm_unlink(name_uid);
	if(ret_unlink == -1) {
		perror("\nErro no unlink da memoria partilhada\n");
		exit(1);
	}

	int ret_unmap = munmap(ptr, size);
	if(ret_unmap == -1) {
		perror("\nErro no unmap de memoria partilhada\n");
		exit(2);
	}

	//
	//so_memory_destroy(name, ptr, size);
	//==============================================
}

//******************************************
// MEMORIA_DESTRUIR
//
void memory_destroy_all() {
	//==============================================
	// DESTRUIR MAPEAMENTO E NOME DE PÁGINAS DE MEMÓRIA
	//
	// utilizar a função genérica memory_destroy(char *,void *,int)
	//

	int ptr_size;
	int buff_size;

	buff_size = Config.OPERATIONS * sizeof(int);
	memory_destroy("shm_capacidade_portuaria",
												Config.capacidade_portuaria, buff_size);

	ptr_size  = sizeof(Pointer);
	memory_destroy("shm_agendamento_ptr", BAgendamento.ptr, ptr_size);
	memory_destroy("shm_agendamento_buffer", BAgendamento.buffer, buff_size);

	buff_size = Config.BUFFER_PEDIDO * sizeof(Operation);
	memory_destroy("shm_pedido_ptr", BPedido.ptr, ptr_size);
	memory_destroy("shm_pedido_buffer", BPedido.buffer, buff_size);

	ptr_size  = Config.BUFFER_DESCRICAO * sizeof(int);
	memory_destroy("shm_descricao_ptr", BDescricao.ptr, ptr_size);
	memory_destroy("shm_descricao_buffer", BDescricao.buffer, buff_size);

	buff_size = Config.OPERATIONS * Config.EMPRESAS * sizeof(int);
	memory_destroy("shm_scheduler", Schedule.ptr, buff_size);
	memory_destroy("shm_op_em",Ind.agendamentos_entregues_por_empresas,buff_size);

	free(Ind.capacidade_inicial_portuaria);
	free(Ind.pid_clientes);
	free(Ind.pid_intermediarios);
	free(Ind.pid_empresas);
	free(Ind.clientes_servidos_por_intermediarios);
	free(Ind.clientes_servidos_por_empresas);
	free(Ind.servicos_recebidos_por_clientes);

	//
	//so_memory_destroy_all();
	//==============================================
}

//******************************************
// memory_request_d_write
//
void memory_request_d_write(int id, struct operation *operacao) {
	prodcons_request_d_produce_begin();

	// registar hora do pedido de servico
	time_register(&operacao->time_descricao);

	//==============================================
	// ESCREVER DESCRICAO DE OPERACOES NO BUFFER DESCRICOES
	//
	// procurar posicao vazia no buffer BDescricao
	// copiar conteudo relevante da estrutura operacao para
	// a posicao BDescricao.ptr do buffer BDescricao
	// conteudo: cliente, id, time_descricao
	// colocar BDescricao.ptr[n] = 1 na posicao respetiva
	//

	int i = 0;

	while(BDescricao.ptr[i] == 1 && i < Config.BUFFER_DESCRICAO) {
		i++;
	}

	BDescricao.buffer[i].cliente        = (*operacao).cliente;
	BDescricao.buffer[i].id             = (*operacao).id;
	BDescricao.buffer[i].time_descricao = (*operacao).time_descricao;

	BDescricao.ptr[i] = 1;

	//
	//so_memory_request_d_write(id, operacao);
	//==============================================

	prodcons_request_d_produce_end();

	// informar INTERMEDIARIO de pedido de operacao
	control_cliente_submete_descricao(id);

	// log
	file_write_log_file(1, id);
}
//******************************************
// memory_request_d_read
//
int memory_request_d_read(int id, struct operation *operacao) {
	// testar se existem clientes e se o SO_AdmPor está open
	if (control_intermediario_esperapor_descricao(id) == 0)
		return 0;

	prodcons_request_d_consume_begin();

	//==============================================
	// LER DESCRICAO DO BUFFER DE DESCRICOES
	//
	// procurar descrição de operação para a empresa no buffer BDescricao
	// copiar conteudo relevante da posicao encontrada
	// no buffer BDescricao para operacao
	// conteudo: cliente, id, time_descricao
	// colocar BDescricao.ptr[n] = 0 na posicao respetiva
	//

	int i = 0;

	while(BDescricao.ptr[i] == 0 && i < Config.BUFFER_DESCRICAO) {
		i++;
	}

	(*operacao).cliente        = BDescricao.buffer[i].cliente;
	(*operacao).id             = BDescricao.buffer[i].id;
	(*operacao).time_descricao = BDescricao.buffer[i].time_descricao;

	BDescricao.ptr[i] = 0;

	//
	//so_memory_request_d_read(id, operacao);
	//==============================================

	// testar se existe capacidade portuaria para servir cliente
	if (prodcons_update_capacidade_portuaria(operacao->id) == 0) {
		operacao->disponibilidade = 0;
		prodcons_request_d_consume_end();
		return 2;
	} else
		operacao->disponibilidade = 1;

	prodcons_request_d_consume_end();

	// log
	file_write_log_file(2, id);

	return 1;
}

//******************************************
// memory_request_r_write
//
void memory_request_r_write(int id, struct operation *operacao) {
	int pos, empresa;

	prodcons_request_r_produce_begin();

	// decidir a que empresa se destina
	empresa = scheduler_get_empresa(id, operacao->id);

	//==============================================
	// ESCREVER PEDIDO NO BUFFER DE PEDIDOS DE OPERACOES
	//
	// copiar conteudo relevante da estrutura operacao para
	// a posicao BDescricao.ptr->in do buffer BPedido
	// conteudo: cliente, id, disponibilidade, intermediario, empresa, time_descricao
	// e atualizar BPedido.ptr->in
	//

	int i = (*BPedido.ptr).in;

	BPedido.buffer[i].cliente         = (*operacao).cliente;
	BPedido.buffer[i].id              = (*operacao).id;
	BPedido.buffer[i].disponibilidade = (*operacao).disponibilidade;
	BPedido.buffer[i].intermediario   = (*operacao).intermediario;
	BPedido.buffer[i].empresa         = (*operacao).empresa;
	BPedido.buffer[i].time_descricao  = (*operacao).time_descricao;

	(*BPedido.ptr).in = (i + 1) % Config.BUFFER_PEDIDO;

	pos = (*BPedido.ptr).in;

	//
	//pos = so_memory_request_r_write(id, operacao, empresa);
	//==============================================

	prodcons_request_r_produce_end();

	// informar empresa respetiva de pedido de operacao
	control_intermediario_submete_pedido(empresa);

	// registar hora pedido (operacao)
	time_register(&BAgendamento.buffer[pos].time_pedido);

	// log
	file_write_log_file(3, id);
}
//******************************************
// memory_request_r_read
//
int memory_request_r_read(int id, struct operation *operacao) {
	// testar se existem pedidos e se o SO_AdmPor está open
	if (control_empresa_esperapor_pedido(id) == 0)
		return 0;

	prodcons_request_r_consume_begin();

	//==============================================
	// LER PEDIDO DO BUFFER DE PEDIDOS DE OPERACOES
	//
	// copiar conteudo relevante dessa posicao BPedido.ptr->out
	// do buffer BPedido para a estrutura operacao
	// conteudo: cliente, id, disponibilidade, intermediario, time_descricao, time_pedido
	// e atualizar BPedido.ptr->out
	//

	int i = (*BPedido.ptr).out;

	(*operacao).cliente         = BPedido.buffer[i].cliente;
	(*operacao).id              = BPedido.buffer[i].id;
	(*operacao).disponibilidade = BPedido.buffer[i].disponibilidade;
	(*operacao).intermediario   = BPedido.buffer[i].intermediario;
	(*operacao).time_descricao  = BPedido.buffer[i].time_descricao;
	(*operacao).time_pedido     = BPedido.buffer[i].time_pedido;

	(*BPedido.ptr).out = (i + 1) % Config.BUFFER_PEDIDO;

	//
	//so_memory_request_r_read(id, operacao);
	//==============================================

	prodcons_request_r_consume_end();

	// log
	file_write_log_file(4, id);

	return 1;
}

//******************************************
// memory_response_s_write
//
void memory_response_s_write(int id, struct operation *operacao) {
	int pos;

	prodcons_response_s_produce_begin();

	//==============================================
	// ESCREVER AGENDAMENTO NO BUFFER DE AGENDAMENTO
	//
	// copiar conteudo relevante da estrutura operacao para
	// a posicao BAgendamento.ptr->in do buffer BAgendamento
	// conteudo: cliente, id, disponibilidade, intermediario, empresa, time_descricao, time_pedido
	// e atualizar BAgendamento.ptr->in
	//

	int i = (*BAgendamento.ptr).in;

	BAgendamento.buffer[i].cliente         = (*operacao).cliente;
	BAgendamento.buffer[i].id              = (*operacao).id;
	BAgendamento.buffer[i].disponibilidade = (*operacao).disponibilidade;
	BAgendamento.buffer[i].intermediario   = (*operacao).intermediario;
	BAgendamento.buffer[i].empresa         = (*operacao).empresa;
	BAgendamento.buffer[i].time_descricao  = (*operacao).time_descricao;
	BAgendamento.buffer[i].time_pedido     = (*operacao).time_pedido;

	(*BAgendamento.ptr).in = (i + 1) % Config.BUFFER_AGENDAMENTO;

	pos = (*BAgendamento.ptr).in;

	//
	//pos = so_memory_response_s_write(id, operacao);
	//==============================================

	prodcons_response_s_produce_end();

	// informar cliente de que a agendamento esta pronta
	control_empresa_submete_agendamento(operacao->cliente);

	// registar hora pronta (agendamento)
	time_register(&BAgendamento.buffer[pos].time_agendamento);

	// log
	file_write_log_file(5, id);
}
//******************************************
// memory_response_s_read
//
void memory_response_s_read(int id, struct operation *operacao) {
	// aguardar agendamento
	control_cliente_esperapor_agendamento(operacao->cliente);

	prodcons_response_s_consume_begin();

	//==============================================
	// LER AGENDAMENTO DO BUFFER DE AGENDAMENTO
	//
	// copiar conteudo relevante da posicao BAgendamento.ptr->out
	// do buffer BAgendamento para a estrutura operacao
	// conteudo: cliente, disponibilidade, intermediario, empresa, time_descricao, time_pedido, time_agendamento
	// e atualizar BAgendamento.ptr->out
	//

	int i = (*BAgendamento.ptr).out;

	(*operacao).cliente          = BAgendamento.buffer[i].cliente;
	(*operacao).disponibilidade  = BAgendamento.buffer[i].disponibilidade;
	(*operacao).intermediario    = BAgendamento.buffer[i].intermediario;
	(*operacao).empresa          = BAgendamento.buffer[i].empresa;
	(*operacao).time_descricao   = BAgendamento.buffer[i].time_descricao;
	(*operacao).time_pedido      = BAgendamento.buffer[i].time_pedido;
	(*operacao).time_agendamento = BAgendamento.buffer[i].time_agendamento;

	(*BAgendamento.ptr).out = (i + 1) % Config.BUFFER_AGENDAMENTO;

	//
	//so_memory_response_s_read(id, operacao);
	//==============================================

	prodcons_response_s_consume_end();

	// log
	file_write_log_file(6, id);
}

//******************************************
// MEMORIA_CRIAR_INDICADORES
//
void memory_create_statistics() {
	//==============================================
	// CRIAR ZONAS DE MEMÓRIA PARA OS INDICADORES
	//
	// criação dinâmica de memória
	// para cada campo da estrutura indicadores
	//

	int *buffer;

	buffer = calloc(Config.OPERATIONS, sizeof(int));
	Ind.capacidade_inicial_portuaria = buffer;

	buffer = calloc(Config.CLIENTES, sizeof(int));
	Ind.pid_clientes = buffer;

	buffer = calloc(Config.INTERMEDIARIO, sizeof(int));
	Ind.pid_intermediarios = buffer;

	buffer = calloc(Config.EMPRESAS, sizeof(int));
	Ind.pid_empresas = buffer;

	buffer = calloc(Config.INTERMEDIARIO, sizeof(int));
	Ind.clientes_servidos_por_intermediarios = buffer;

	buffer = calloc(Config.EMPRESAS, sizeof(int));
	Ind.clientes_servidos_por_empresas = buffer;

	buffer = calloc(Config.OPERATIONS, sizeof(int));
	Ind.servicos_recebidos_por_clientes = buffer;


	buffer = memory_create("shm_op_em",
													Config.OPERATIONS * Config.EMPRESAS * sizeof(int));
	Ind.agendamentos_entregues_por_empresas = buffer;

	//
	//so_memory_create_statistics();
	// iniciar indicadores relevantes:
	// - Ind.capacidade_inicial_portuaria (c/ Config.capacidade_portuaria respetivo)
	// - Ind.clientes_servidos_por_intermediarios (c/ 0)
	// - Ind.clientes_servidos_por_empresas (c/ 0)
	// - Ind.serviços_recebidos_por_clientes (c/ 0)
	//

	for(int i = 0; i < Config.OPERATIONS; i++) {
		Ind.capacidade_inicial_portuaria[i] = Config.capacidade_portuaria[i];
	}

	//
	//so_memory_prepare_statistics();
	//==============================================
}
