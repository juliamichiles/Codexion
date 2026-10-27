#include "codexion.h"

int	init_simulation(t_simulation *sim, const t_args *args)
{
	if (!sim || !args)
		return (1);
	sim->start_time_ms = get_current_time(); // not sure that's the right place
	sim->coders = NULL; //bc defensive programming
	sim->dongles = NULL; //unnecessary in my opinion...
	sim->stopped = 0;
        sim->finished_coders = 0;
	sim->args = *args;
	sim->dongles = init_dongles(args->number_of_coders);
	if (!sim->dongles)
		return (1);
	sim->coders = init_coders(sim, args->number_of_coders);
	if (!sim->coders)
	{
		dongle_cleanup(&sim->dongles, args->number_of_coders);
		return (1);
	}
	assign_dongles(sim);
	if (init_sim_mutexes(sim) != 0)
	{
		free(sim->coders);
		sim->coders = NULL;
		dongle_cleanup(&sim->dongles, args->number_of_coders);
		return (1);
	}
	return (0);
}

int	destroy_simulation(t_simulation *sim)
{
	// NOTE: Only to be called if init_simulation succeds
	if (!sim)
		return (1);
	free(sim->coders);
	sim->coders = NULL;
	dongle_cleanup(&sim->dongles, sim->args.number_of_coders);
	handle_mutex(&sim->log_mutex, DESTROY);
        handle_mutex(&sim->state_mutex, DESTROY);
        handle_mutex(&sim->scheduler_mutex, DESTROY);
	pthread_cond_destroy(&sim->scheduler_cond);
	// Should also free the simulation itself and set ptr to NULL?
	// If so *sim must become a double ptr
	return (0);
}
/*
int	simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	// FIXME: check for number_of_compiles == 0?
	// or other stuff that can't be 0, but maybe stop that at parser already
	if (sim->number_of_coders == 1)
		; // TODO: Idk how we're supposed to handle that...
	
}*/
