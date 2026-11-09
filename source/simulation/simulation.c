#include "codexion.h"

int	init_simulation(t_simulation *sim, const t_args *args)
{
	if (!sim || !args)
		return (1);
	sim->start_time_ms = 0; // maybe remove this line entirely
	sim->coders = NULL; //bc defensive programming
	sim->dongles = NULL; //unnecessary in my opinion...
	sim->stopped = false;
    sim->finished_coders = 0;
	sim->args = *args;
	sim->dongles = init_dongles(args->number_of_coders, args->scheduler);
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

/* NOTE: Only to be called if init_simulation succeds*/
int	destroy_simulation(t_simulation *sim)
{
	if (!sim)
		return (1);
	free(sim->coders);
	sim->coders = NULL;
	dongle_cleanup(&sim->dongles, sim->args.number_of_coders);
	handle_mutex(&sim->log_mutex, DESTROY);
    handle_mutex(&sim->state_mutex, DESTROY);
    handle_mutex(&sim->scheduler_mutex, DESTROY);
	pthread_cond_destroy(&sim->scheduler_cond);
	return (0);
}

static void init_start_times(t_simulation *sim)
{
    int i;

    i = 0;
    sim->start_time_ms = get_current_time();
    while (i < sim->args.number_of_coders)
    {
        sim->coders[i].last_compile_start = sim->start_time_ms;
        i++;
    }
}

static int  start_coders(t_simulation *sim)
{
    int i;

    i = 0;
    while (i < sim->args.number_of_coders)
    {
        if (handle_thread(&sim->coders[i].thread, coder_routine,
                    &sim->coders[i], CREATE) != 0)
            break ;
        i++;
    }
    return (i);
}

int run_simulation(t_simulation *sim)
{
    int created;
    int i;

    i = 0;
    init_start_times(sim);
    if (handle_thread(&sim->monitor_thread, monitor_routine, sim, CREATE))
    {
        p_error("Failed to create monitor thread.");
        return (1);
    }
    created = start_coders(sim);
    if (created < sim->args.number_of_coders)
    {
        p_error("Failed to create coder thread.");
        stop_simulation(sim);
    }
    while (i < created)
        handle_thread(&sim->coders[i++].thread, NULL, NULL, JOIN);
    handle_thread(&sim->monitor_thread, NULL, NULL, JOIN);
    return (created < sim->args.number_of_coders);
}
