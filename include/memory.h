#ifndef MEMORY_H_GUARD
#define MEMORY_H_GUARD

// Nomes usados na criação de zonas de memoria partilhada
#define STR_SHM_CAPACIDADE_PORTUARIA 	"shm_capacidade_portuaria"
#define STR_SHM_AGENDAMENTO_PTR 			"shm_agendamento_ptr"
#define STR_SHM_AGENDAMENTO_BUFFER 		"shm_agendamento_buffer"
#define STR_SHM_PEDIDO_PTR 			"shm_pedido_ptr"
#define STR_SHM_PEDIDO_BUFFER 		"shm_pedido_buffer"
#define STR_SHM_DESCRICAO_PTR 			"shm_descricao_ptr"
#define STR_SHM_DESCRICAO_BUFFER 			"shm_descricao_buffer"
#define STR_SHM_SCHEDULER 				"shm_scheduler"
#define STR_SHM_OPREMPRESAS				"shm_operations_empresas"

// estrutura que contêm os ponteiros para um buffer circular
struct pointers {
	int in;  // ponteiro de escrita
	int out; // ponteiro de leitura
};

// estrutura onde são guardadas as descricoes de operações do cliente para o intermediario
struct request_d {
	struct operation *buffer;   // ponteiro para a lista de descrições de operação
	int *ptr;             // ponteiro para a lista de inteiros que indicam
						  // se a posição respetiva está livre ou ocupada
};

//====================================================================================
// estrutura onde são guardados os pedidos de operação do intermediario para a empresa
struct request_r {
	struct operation *buffer; // ponteiro para a lista de pedidos de operação
	struct pointers *ptr;             // ponteiro para a estrutura pointers
};
//====================================================================================

//====================================================================================
// estrutura onde a empresa disponibiliza os agendamentos
struct response_s {
	struct operation *buffer; // ponteiro para a lista de pedidos de operação
	struct pointers *ptr;             // ponteiro para a estrutura pointers
};
//====================================================================================

void * memory_create(char*, int);
void memory_create_capacidade_portuaria();
void memory_create_buffers();
void memory_create_scheduler();
void memory_destroy_all();
void memory_request_d_write(int, struct operation *);
int  memory_request_d_read(int, struct operation *);
void memory_request_r_write(int, struct operation *);
int  memory_request_r_read(int, struct operation *);
void memory_response_s_write(int, struct operation *);
void memory_response_s_read(int, struct operation *);
void memory_create_statistics();
void memory_destroy(char * name, void * ptr, int size);

#endif
