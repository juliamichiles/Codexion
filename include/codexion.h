#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "codexion.h"

// Utilitary:
int	ft_isspace(char c);
int	ft_isnum(char c);

// Thread-related helpers:
void    handle_mutex(t_mtx *mutex, t_op code);
// FIXME: Is this line-break ok? looks ugly...
void    handle_thread(pthread_t *thread, void *(*f)(void *)
		, void *data, t_op code);
long	get_long(t_mtx *mutex, long *value);
void	set_long(t_mtx *mutex, long *dest, long value);
void	set_bool(t_mtx *mutex, bool *dest, bool value);
bool	get_bool(t_mtx *mutex, bool *value);

// Parsing:
t_args	*parse_args(char *args[]);

// Initialization:
int	init_simulation(t_simulation *sim, const t_args *args);
void	destroy_simulation(t_simulation *sim);

#endif
