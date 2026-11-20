/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliatav <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 18:45:23 by juliatav          #+#    #+#             */
/*   Updated: 2026/11/20 18:45:37 by juliatav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*Sleeps for ~0.5 milisecons at a time while the given time in miliseconds
 * hasn't been reached and the simulation hasn't ended (tested with 200 
 * to 1500)*/
void	ms_sleep(t_simulation *sim, t_llong ms)
{
	t_llong	end;

	end = get_current_time() + ms;
	while (!simulation_ended(sim) && get_current_time() < end)
		usleep(300);
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
	int	i;

	if (!dongles || !*dongles)
		return ;
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
	handle_mutex(&sim->scheduler_mutex, LOCK);
	pthread_cond_broadcast(&sim->scheduler_cond);
	handle_mutex(&sim->scheduler_mutex, UNLOCK);
}
