#include "codexion.h"

// - Copy the parsed arguments into sim->args.
// - Allocate the array of coders and the array of dongles.
// - Initialize the shared mutexes and condition variable.
// - Initialize each coder and dongle.
// - Connect each coder to its left and right dongles in a circle.

t_dongle *init_dongles(int n_coders)
{
	int		i;
	t_dongle	dongles;

	i = 0;
	dongles = malloc(n_coders * sizeof(t_dongle));
	if (!dongles)
	{
		p_error("Dongles memory allocation failled.");
		return (NULL);
	}
	while (i < n_coders)
	{
		dongles[i] = init_dongle(); // TODO
		if (!dongles)
			// Do we really need this?
                        // If the new function allocs. mem. check for NULL
                        // free previous coders and the whole thing than
                        // return NULL
		i++;
	}
	return (dongles);
}

t_coder	*init_coders(t_simulation *sim, int n_coders)
{
	int	i;
	t_coder	*coders;

	i = 0;
	coders = malloc(n_coders * sizeof(t_coder));
	if (!coders)
		//FIXME: Write a better error message
		p_error("Coders memory allocation failled.");
		return (NULL);
	}
	while (i < n_coders)
	{
		coders[i] = init_coder(); // TODO
		if (!coders[i])
			// Do we really need this?
			// If the new function allocs. mem. check for NULL
			// free previous coders and the whole thing than
			// return NULL
		i++;
	}
	return (coders);
}

void	init_simulation(t_simulation *sim, const t_args *args)
{
	// TODO: maybe change return type so we can flag errors to caller
	if (!sim || !args)
		return;
	sim->args = *args;
	sim->coders = init_coders(args->number_of_coders, sim);
	if (!sim->coders)
		return;
	sim->dongles = init_dongles(args->number_of_coders);
	if (!sim->dongles)
	{
		free(sim->coders) //Right?
		return;
	}
	sim->monitor_thread = // TODO
	sim->log_mutex = // TODO
	sim->log_mutex = // TODO
	sim->scheduler = //TODO
	sim->stopped = 0;
	sim->finished_coders = 0;
}

int	destroy_simulation(t_simulation *sim)
{
	int	id;
	
	id = 0;
	while (id < sim->number_of_coders)
		//cleanup
		id++;
}
