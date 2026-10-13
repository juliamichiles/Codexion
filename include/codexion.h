#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

// Structures:
typedef struct s_args
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles;
	int	dongle_cooldown;
	int	scheduler;
}		t_args;

typedef enum	e_op
{
		LOCK,
		UNLOCK,
		INIT,
		DESTROY,
		CREATE,
		JOIN,
		DETACH
}		t_op;

typedef enum	e_coder_state
{
		WAITING_FOR_DONGLES,
		COMPILING,
		DEBUGGING,
		REFACTORING,
		CODER_DONE
}		t_coder_state;

typedef struct	s_simulation t_simulation;

typedef struct		s_dongle
{
	int		id;
	pthread_mutex_t	mutex;
	long long	avaliable_at_ms;
}			t_dongle;

typedef struct	s_coder
{
	int		id;
	pthread_t	thread;
	t_coder_state	state;

	t_dongle	*left_dongle;
	t_dongle	*right_dongle;

	long long	last_compile_start;
	int 		compile_count;

	t_simulation	*sim;
}	t_coder;


struct	s_simulation
{
	t_args			args;
	t_coder			*coders;
	t_dongle		*dongles;

	pthread_t		monitor_thread;

	pthread_mutex_t	log_mutex;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;

	int		stopped;
	int		finshed_coders;
}
// Utilitary:
int	ft_isspace(char c);
int	ft_isnum(char c);

// Thread-related helpers:
void    handle_mutex(pthread_mutex_t *mutex, t_op code);
void    handle_thread(pthread_t *thread, void *(*f)(void *)
		, void *data, t_op code);
// Parsing:
t_args	*parse_args(char *args[]);

// Initialization:
int	init_simulation(t_simulation *sim, const t_args *args);
void	destroy_simulation(t_simulation *sim);
#endif
