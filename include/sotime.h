#ifndef TIME_H_GUARD
#define TIME_H_GUARD

void time_begin(long);
void time_destroy(long);
void time_setup_alarm();
void time_write_log_timed(int);
double time_difference(struct timespec, struct timespec);
double time_untilnow();
void time_register(struct timespec *);
void time_processing_order();
float time_average_statistic(int id);
float time_minimum_statistic(int id);
float time_maximum_statistic(int id);

#endif
