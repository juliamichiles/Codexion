#ifndef CODEXION_H
# define CODEXION_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include "structures.h"

// Utilitary:
int	ft_isspace(char c);
int	ft_isnum(char c);
void	p_error(char *message);

// Thread-related helpers:
int	handle_mutex(t_mtx *mutex, t_op code);

// FIXME: Is this line-break ok? looks ugly...
int    handle_thread(pthread_t *thread, void *(*f)(void *)
		, void *data, t_op code);
void	dongle_cleanup(t_dongle **dongles, int size);
long	get_long(t_mtx *mutex, long *value);
void	set_long(t_mtx *mutex, long *dest, long value);
void	set_bool(t_mtx *mutex, bool *dest, bool value);
bool	get_bool(t_mtx *mutex, bool *value);

// Parsing:
t_args	*parse_args(char *args[]);

// Initialization:
t_dongle	*init_dongles(int n_coders);
t_coder		*init_coders(t_simulation *sim, int n_coders);
int		init_sim_mutexes(t_simulation *sim);
void		assign_dongles(t_simulation *sim);

// Simulation:
int	init_simulation(t_simulation *sim, const t_args *args);
int	destroy_simulation(t_simulation *sim);


#endif
