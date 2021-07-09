#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD

// estrutura que representa uma operação nas tres etapas: descricao, pedido e agendamento
struct operation {
	int id;  		            		// identificador do tipo de operação solicitado
	int disponibilidade;           		// operacao: 0 - indisponivel, 1 - disponivel
	int cliente;     	            	// id do cliente que solicitou a operação
	int intermediario;         			// id do intermediario que atendeu o cliente
	int empresa;  	            		// id da empresa que efetuou a operação
	struct timespec time_descricao; 	// hora a que a descrição de operação foi submetida ao intermediario (registada pelo cliente)
	struct timespec time_pedido;  	// hora a que o pedido de operação foi submetida a empresa (registada pela intermediario)
	struct timespec time_agendamento; 		// hora a que o agendamento da operação foi elaborada pela empresa (registada pela empresa)
};

// estrutura com valores dos parâmetros do ficheiro de teste
struct configuration {
	char* list_operacoes;      		// linha com capacidade de operação
	char* list_clientes;    		// linha com operação pretendido por cada cliente
	char* nr_operacoes;				// numero de operações que cada cliente pode efetuar
	char* list_intermediarios;		// linha com os intermediarios (nomes nao interessam)
	char* list_empresas; 			// linha com as operações por empresa separadas por virgula
	char* list_buffers;     		// linha com capacidade dos tres buffers
	int OPERATIONS;            		// nº de operações disponiveis
	int N;            				// nº de operações que cada cliente pode realizar
	int CLIENTES;          			// nº de clientes
	int INTERMEDIARIO;        		// nº de intermediario
	int EMPRESAS;       			// nº de empresa
	int BUFFER_DESCRICAO; 			// capacidade do buffer de descrições de operação
	int BUFFER_PEDIDO;  			// capacidade do buffer de pedidos de operação
	int BUFFER_AGENDAMENTO;     	// capacidade do buffer de agendamentos de operação
	int* capacidade_portuaria;      // vetor com capacidade por tipo de operação
};

struct op_time {
	int cliente;
	float time;
};

// estrutura de registo de indicadores de funcionamento do SO_AdmPor
struct statistics {
	int *capacidade_inicial_portuaria;
	int *pid_clientes;
	int *pid_intermediarios;
	int *pid_empresas;
	int *clientes_servidos_por_intermediarios;
	int *clientes_servidos_por_empresas;
	int *agendamentos_entregues_por_empresas;
	int *servicos_recebidos_por_clientes;
	struct op_time *tempos;
};

#endif
