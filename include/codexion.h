#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <stdio.h>
# include <errno.h>
# include <unistd.h>
# include <sys/time.h>
# include "structures.h"

/* Utilitary:*/
int			ft_isspace(char c);
int			ft_isnum(char c);
void		p_error(char *message);
void		*cleanup(void *memory, void *message);
t_llong		get_current_time(void);
t_llong		get_elapsed_time(t_llong start_time);
int			handle_mutex(t_mtx *mutex, t_op code);
int			handle_thread(pthread_t *thread, void *(*f)(void *),
				void *data, t_op code);
void		dongle_cleanup(t_dongle **dongles, int size);
void		ms_sleep(t_simulation *sim, t_llong ms);
t_llong		get_llong(t_mtx *mutex, t_llong *value);
void		set_llong(t_mtx *mutex, t_llong *dest, t_llong value);
void		set_bool(t_mtx *mutex, bool *dest, bool value);
bool		get_bool(t_mtx *mutex, bool *value);

/* Parsing:*/
t_args		*parse_args(char *args[]);

/* Initialization:*/
t_dongle	*init_dongles(int n_coders, int policy);
t_coder		*init_coders(t_simulation *sim, int n_coders);
int			init_sim_mutexes(t_simulation *sim);
void		assign_dongles(t_simulation *sim);

/* Simulation:*/
int			init_simulation(t_simulation *sim, const t_args *args);
int			destroy_simulation(t_simulation *sim);
int			run_simulation(t_simulation *sim);
void		stop_simulation(t_simulation *sim);
bool		simulation_ended(t_simulation *sim);

/* Heap:*/
t_heap		*init_heap(int capacity, int policy);
void		heap_destroy(t_heap *heap);
void		heap_push(t_heap *heap, t_llong arrival_time, t_llong deadline,
				int id);
int			heap_pop(t_heap *heap, t_node *out_node);

/* Coders, dongles and monitor:*/
void		init_coder(t_simulation *sim, t_coder *coder, int index);
void		release_dongle(t_coder *coder, t_dongle *dongle);
void		print_status(t_simulation *sim, int coder_id, t_coder_state state);
void		wake_everyone(t_simulation *sim);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int			acquire_dongles(t_coder *c, t_dongle *a, t_dongle *b);
bool		is_ready(t_dongle *d, int id);
void		wait_dongle(t_dongle *d);
void		lock_pair(t_dongle *a, t_dongle *b);
void		unlock_pair(t_dongle *a, t_dongle *b);

#endif
