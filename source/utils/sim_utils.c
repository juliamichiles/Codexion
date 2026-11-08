#include "codexion.h"

/*Sleeps for ~0.5 milisecons at a time while the given time in miliseconds
 * hasn't been reached and the simulation hasn't ended*/
void	ms_sleep(t_simulation *sim, llong ms)
    {
	llong   end;

	end = get_current_time() + ms;
	while (!simulation_ended(sim) && get_current_time() < end)
		usleep(300); // was 500 before
                      // tested 200 300 400 100 and 1500
}

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->nodes);
	free(heap);
}

void	dongle_cleanup(t_dongle **dongles, int size)
{
	int i;

	if (!dongles || !*dongles)
		return;
	i = 0;
	while (i < size)
	{
		heap_destroy((*dongles)[i].heap);
		pthread_cond_destroy(&(*dongles)[i].cond);
		handle_mutex(&(*dongles)[i].mutex, DESTROY);
		i++;
	}
	free(*dongles);
	*dongles = NULL;
}

void	stop_simulation(t_simulation *sim)
{
	set_bool(&sim->state_mutex, &sim->stopped, true);
	wake_everyone(sim);
}

void	wake_everyone(t_simulation *sim)
{
	int i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		handle_mutex(&sim->dongles[i].mutex, LOCK);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		handle_mutex(&sim->dongles[i].mutex, UNLOCK);
		i++;
	}
}
