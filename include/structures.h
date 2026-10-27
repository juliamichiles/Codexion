#ifndef STRUCTURES_H
# define STRUCTURES_H

#include <pthread.h>
#include <stdbool.h>

typedef pthread_mutex_t t_mtx;
typedef long long	llong;

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
		TAKEN_DONGLE,
		COMPILING,
		DEBUGGING,
		REFACTORING,
		BURNED_OUT,
		CODER_DONE
}		t_coder_state;

typedef struct	s_simulation t_simulation;

typedef struct		s_dongle
{
	int		id;
	t_mtx 		mutex;
	llong		avaliable_at_ms;
}			t_dongle;

typedef struct	s_coder
{
	int		id;
	pthread_t	thread;
	t_coder_state	state;

	t_dongle	*left_dongle;
	t_dongle	*right_dongle;

	llong		last_compile_start;
	int		compile_count;

	t_simulation	*sim;
}	t_coder;

struct	s_simulation
{
	t_args		args;
	t_coder		*coders;
	t_dongle	*dongles;

	pthread_t	monitor_thread;
	llong		start_time_ms;

	t_mtx		    log_mutex;
	t_mtx 		    state_mutex;
	t_mtx 		    scheduler_mutex;
	pthread_cond_t	scheduler_cond;

	int		stopped;
	int		finished_coders;
};

typedef struct	s_node
{
	int	    coder_id;
	llong	arrival_time;
	llong	deadline;
}		    t_node;

typedef struct	s_heap
{
	t_node	*nodes;
	int	    capacity; // total max capacity allocated
	int	    size; // current size/occupation
	int 	policy; // 1 FIFO / 2 EDF

}           t_heap;
#endif
