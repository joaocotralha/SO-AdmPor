#ifndef SO_H_GUARD
#define SO_H_GUARD

void so_write_statistics();

/* inih -- simple .INI file parser

 inih is released under the New BSD license (see LICENSE.txt). Go to the project
 home page for more info:
 http://code.google.com/p/inih/


 Parse given INI-style file. May have [section]s, name=value pairs
 (whitespace stripped), and comments starting with ';' (semicolon). Section
 is "" if name=value pair parsed before any section heading.

 For each name=value pair parsed, call handler function with given user
 pointer as well as section, name, and value (data only valid for duration
 of handler call). Handler should return nonzero on success, zero on error.

 Returns 0 on success, line number of first error on parse error, or -1 on
 file open error.
 */
int ini_parse(const char* filename,
		int (*handler)(void* user, const char* section, const char* name,
				const char* value), void* user);
int ini_parse_file(FILE* file,
		int (*handler)(void*, const char*, const char*, const char*),
		void* user);

/* Nonzero to allow multi-line value parsing, in the style of Python's
 ConfigParser. If allowed, ini_parse() will call the handler with the same
 name for each subsequent line parsed. */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE 1
#endif

//***********************************
// Funções de apoio ao desenvolvimento do trabalho
//***********************************
// MAIN
//
void so_main_intermediario(int);
void so_main_empresas(int);
void so_main_cliente(int);
long so_main_args(int, char**, char **, char **, char **);
void so_main_wait_clientes();
void so_main_wait_intermediarios();
void so_main_wait_empresas();
//***********************************
// ESCALONADOR
//
void so_scheduler_begin(int, int);
int so_scheduler_get_empresas(int, int);
//***********************************
// MEMORIA
//
void *so_memory_create(char *, int);
void so_memory_create_capacidade_portuaria();
void so_memory_create_buffers();
void so_memory_create_scheduler();
void so_memory_destroy(char *, void *, int);
void so_memory_destroy_all();
void so_memory_request_d_write(int, struct operation *);
void so_memory_request_d_read(int, struct operation *);
int  so_memory_request_r_write(int, struct operation *, int);
void so_memory_request_r_read(int, struct operation *);
int  so_memory_response_s_write(int, struct operation *);
void so_memory_response_s_read(int, struct operation *);
void so_memory_create_statistics();
void so_memory_prepare_statistics();
//***********************************
// PRODCONS
//
sem_t *so_semaphore_create(char *, int);
void so_prodcons_create_capacidade_portuaria();
void so_prodcons_create_buffers();
void so_semaphore_destroy(char *, sem_t *);
void so_prodcons_destroy();
void so_prodcons_request_d_produce_begin();
void so_prodcons_request_d_produce_end();
void so_prodcons_request_d_consume_begin();
void so_prodcons_request_d_consume_end();
void so_prodcons_request_r_produce_begin();
void so_prodcons_request_r_produce_end();
void so_prodcons_request_r_consume_begin();
void so_prodcons_request_r_consume_end();
void so_prodcons_response_s_produce_begin();
void so_prodcons_response_s_produce_end();
void so_prodcons_response_s_consume_begin();
void so_prodcons_response_s_consume_end();
void so_prodcons_buffers_begin();
void so_prodcons_buffers_end();
int  so_prodcons_update_capacidade_portuaria(int);
//***********************************
// CONTROLO
//
void so_cond_create(pthread_cond_t *, pthread_mutex_t *);
void so_control_create();
void so_control_create_opening();
void so_control_create_wakeup_cliente();
void so_control_create_wakeup_empresa();
void so_control_create_wakeup_intermediario();
void so_cond_destroy(pthread_cond_t *, pthread_mutex_t *);
void so_control_destroy();
void so_control_destroy_opening();
void so_control_destroy_wakeup_cliente();
void so_control_destroy_wakeup_empresa();
void so_control_destroy_wakeup_intermediario();
void so_control_open_soadmpor();
void so_control_close_soadmpor();
void so_control_cliente_submete_descricao(int);
int  so_control_intermediario_esperapor_descricao(int);
void so_control_intermediario_submete_pedido(int);
int  so_control_empresa_esperapor_pedido(int);
void so_control_empresa_submete_agendamento(int);
void so_control_cliente_esperapor_agendamento(int);
//***********************************
// FICHEIRO
//
void so_file_begin(char *, char *, char *);
void so_file_destroy();
void so_file_open_file_in();
void so_file_count_operacoes();
void so_file_read_capacidade_portuaria();
void so_file_count_clientes();
void so_file_read_numero_operacoes();
void so_file_count_intermediarios();
void so_file_count_empresas();
void so_file_read_operacoes();
void so_file_read_capacidade_buffer();
void so_file_open_file_out();
void so_file_open_file_log();
void so_file_write_log_file(int, int, double);
void so_file_write_line(char *);
//***********************************
// TEMPO
//
void so_time_begin(long);
void so_time_setup_alarm();
void so_time_destroy(long);
void so_time_write_log_timed(int);
double so_time_difference(struct timespec, struct timespec);
double so_time_untilnow();
void so_time_register(struct timespec *);
void so_time_processing_order();
float so_time_average_statistic(int id);
float so_time_minimum_statistic(int id);
float so_time_maximum_statistic(int id);
//***********************************
// CLIENTE
//
int so_cliente_operations(int id);


#endif
