#include "codexion.h"

/* - Copy the parsed arguments into sim->args.
* - Allocate the array of coders and the array of dongles.
* - Initialize the shared mutexes and condition variable.
* - Initialize each coder and dongle.
* - Connect each coder to its left and right dongles in a circle.
*/
static int	init_dongle(t_dongle *dongle, int policy, int cap, int i)
{
	dongle->id = i + 1;
	dongle->avaliable_at_ms = 0;
	dongle->held = false;
	dongle->next_ticket = 0;
	dongle->heap = init_heap(cap, policy);
	if (!dongle->heap)
		return (1);
	if (handle_mutex(&dongle->mutex, INIT) != 0)
	{
		heap_destroy(dongle->heap);
		return (1);
	}
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		handle_mutex(&dongle->mutex, DESTROY);
		heap_destroy(dongle->heap);
		return (1);
	}
	return (0);
}

t_dongle	*init_dongles(int n_coders, int policy)
{
	int			i;
	t_dongle	*dongles;

	i = 0;
	dongles = malloc(n_coders * sizeof(t_dongle));
	if (!dongles)
		return (cleanup(NULL, "Dongle allocation failled."));
	while (i < n_coders)
	{
		if (init_dongle(&dongles[i], policy, n_coders, i) != 0)
		{
			dongle_cleanup(&dongles, i);
			return (cleanup(NULL, "Dongle Initialization failled."));
		}
		i++;
	}
	return (dongles);
}

t_coder	*init_coders(t_simulation *sim, int n_coders)
{
	t_coder	*coders;
	int		i;

	i = 0;
	coders = malloc(n_coders * sizeof(t_coder));
	if (!coders)
		cleanup(NULL, ("Coders memory allocation failled."));
	while (i < n_coders)
	{
		init_coder(sim, &coders[i], i);
		i++;
	}
	return (coders);
}

void	assign_dongles(t_simulation *sim)
{
	int	i;
	int	n;

	i = 0;
	n = sim->args.number_of_coders;
	if (n == 1)
	{
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[i];
		return ;
	}
	while (i < n)
	{
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1) % n];
		i++;
	}
}

int	init_sim_mutexes(t_simulation *sim)
{
	if (handle_mutex(&sim->log_mutex, INIT) != 0)
		return (1);
	if (handle_mutex(&sim->state_mutex, INIT) != 0)
	{
		handle_mutex(&sim->log_mutex, DESTROY);
		return (1);
	}
	if (handle_mutex(&sim->scheduler_mutex, INIT) != 0)
	{
		handle_mutex(&sim->log_mutex, DESTROY);
		handle_mutex(&sim->state_mutex, DESTROY);
		return (1);
	}
	if (pthread_cond_init(&sim->scheduler_cond, NULL) != 0)
	{
		handle_mutex(&sim->scheduler_mutex, DESTROY);
		handle_mutex(&sim->state_mutex, DESTROY);
		handle_mutex(&sim->log_mutex, DESTROY);
		return (1);
	}
	return (0);
}
