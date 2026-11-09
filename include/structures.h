#ifndef STRUCTURES_H
# define STRUCTURES_H

# include <pthread.h>
# include <stdbool.h>

typedef pthread_mutex_t	t_mtx;
typedef long long		t_llong;

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

typedef enum e_op
{
	LOCK,
	UNLOCK,
	INIT,
	DESTROY,
	CREATE,
	JOIN,
}	t_op;

typedef enum e_coder_state
{
	WAITING_FOR_DONGLES,
	TAKEN_DONGLE,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	BURNED_OUT,
	CODER_DONE
}	t_coder_state;

typedef struct s_heap		t_heap;
typedef struct s_simulation	t_simulation;

typedef struct s_dongle
{
	int				id;
	t_mtx			mutex;
	pthread_cond_t	cond;
	bool			held;
	t_llong			next_ticket;
	t_llong			avaliable_at_ms;
	t_heap			*heap;
}					t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_coder_state	state;

	t_dongle		*left_dongle;
	t_dongle		*right_dongle;

	t_llong			last_compile_start;
	int				compile_count;

	t_simulation	*sim;
}	t_coder;

struct s_simulation
{
	t_args			args;
	t_coder			*coders;
	t_dongle		*dongles;

	pthread_t		monitor_thread;
	t_llong			start_time_ms;

	t_mtx			log_mutex;
	t_mtx			state_mutex;
	t_mtx			scheduler_mutex;
	pthread_cond_t	scheduler_cond;

	bool			stopped;
	int				finished_coders;
};

typedef struct s_node
{
	int		coder_id;
	t_llong	arrival_time;
	t_llong	deadline;
}			t_node;

struct s_heap
{
	t_node	*nodes;
	int		capacity;
	int		size;
	int		policy;
};

typedef struct s_heap_var
{
	int	smallest;
	int	policy;
	int	right;
	int	left;
	int	i;
}		t_heap_var;

#endif
