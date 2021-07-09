#ifndef PRODCONS_H_GUARD
#define PRODCONS_H_GUARD

// Nomes usados na criacao dos semaforos
#define STR_SEM_CAPACIDADE_PORTUARIA_MUTEX	"sem_capacidade_portuaria_mutex"
#define STR_SEM_DESCRICAO_FULL 				"sem_descricao_full"
#define STR_SEM_DESCRICAO_EMPTY 			"sem_descricao_empty"
#define STR_SEM_DESCRICAO_MUTEX 			"sem_descricao_mutex"
#define STR_SEM_PEDIDO_FULL 				"sem_pedido_full"
#define STR_SEM_PEDIDO_EMPTY 			"sem_pedido_empty"
#define STR_SEM_PEDIDO_MUTEX 			"sem_pedido_mutex"
#define STR_SEM_AGENDAMENTO_FULL 				"sem_agendamento_full"
#define STR_SEM_AGENDAMENTO_EMPTY 				"sem_agendamento_empty"
#define STR_SEM_AGENDAMENTO_MUTEX 				"sem_agendamento_mutex"

struct prodcons {
	// semáforos de controlo do acesso ao buffer entre empresas e clientes
	sem_t *response_s_full, *response_s_empty, *response_s_mutex;
	// semáforos de controlo do acesso ao buffer entre intermediarios e empresas
	sem_t *request_r_full, *request_r_empty, *request_r_mutex;
	// semáforos de controlo do acesso ao buffer entre clientes e intermediarios
	sem_t *request_d_full, *request_d_empty, *request_d_mutex;
	// semaforo para exclusao mutua no acesso a capacidade portuaria
	sem_t *capacidade_portuaria_mutex;
};

sem_t * semaphore_create(char*, int);
void prodcons_create_capacidade_portuaria();
void prodcons_create_buffers();
void prodcons_destroy();
void prodcons_request_d_produce_begin();
void prodcons_request_d_produce_end();
void prodcons_request_d_consume_begin();
void prodcons_request_d_consume_end();
void prodcons_request_r_produce_begin();
void prodcons_request_r_produce_end();
void prodcons_request_r_consume_begin();
void prodcons_request_r_consume_end();
void prodcons_response_s_produce_begin();
void prodcons_response_s_produce_end();
void prodcons_response_s_consume_begin();
void prodcons_response_s_consume_end();
void prodcons_buffers_begin();
void prodcons_buffers_end();
int prodcons_update_capacidade_portuaria(int);

#endif
