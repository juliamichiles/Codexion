#include "codexion.h"

// - Copy the parsed arguments into sim->args.
// - Allocate the array of coders and the array of dongles.
// - Initialize the shared mutexes and condition variable.
// - Initialize each coder and dongle.
// - Connect each coder to its left and right dongles in a circle.

t_dongle *init_dongles(int n_coders, int policy)
{
	int		    i;
    t_dongle	*dongles;

	i = 0;
	dongles = malloc(n_coders * sizeof(t_dongle));
	if (!dongles)
        return (cleanup(dongles, "Heap memory allocation failled."));
    while (i < n_coders)
	{
		dongles[i].id = i + 1;
		dongles[i].avaliable_at_ms = 0;
        dongles[i].heap = init_heap(2, policy);  // is 2 really enough?
        if (!dongles[i].heap)
        {
            dongle_cleanup(&dongles, i); // if too long, make dongle_cleanup call
                                         // cleanup internally
            return (cleanup(NULL, "Dongles memory allocation failled."));
        }
		if (handle_mutex(&dongles[i].mutex, INIT) != 0)
		{
            heap_destroy(dongles[i].heap); // do I really need this? I think so
			dongle_cleanup(&dongles, i);
			return (cleanup(NULL, "Failed to initialize dongle mutex."));
		}
		i++;
	}
	return (dongles);
}

static void	init_coder(t_simulation *sim, t_coder *coder, int index)
{
	// FYI: Doesn't init thread
	coder->id = index + 1;
	coder->state = WAITING_FOR_DONGLES;
	coder->left_dongle = NULL; // for now, assign them later once all
	coder->right_dongle = NULL; // coders and dongles are initialized
	coder->last_compile_start = -1;
	coder->compile_count = 0;
	coder->sim = sim;
}

t_coder	*init_coders(t_simulation *sim, int n_coders)
{
	int	i;
	t_coder	*coders;

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
		// FIXME: NOT SURE this FULLY handles the 1 coder edge case
		// Maybe there's something else to do??
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[i];
		return;
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
