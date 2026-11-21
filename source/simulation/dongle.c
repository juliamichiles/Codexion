/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliatav <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 18:42:29 by juliatav          #+#    #+#             */
/*   Updated: 2026/11/20 18:42:30 by juliatav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	take_pair(t_coder *c, t_dongle *a, t_dongle *b)
{
	heap_remove(a->heap, c->id);
	heap_remove(b->heap, c->id);
	a->held = true;
	b->held = true;
}

int	acquire_dongles(t_coder *c, t_dongle *a, t_dongle *b)
{
	t_simulation	*sim;

	sim = c->sim;
	handle_mutex(&sim->scheduler_mutex, LOCK);
	if (!c->queued)
		queue_request(c);
	c->queued = false;
	while (!simulation_ended(sim) && !can_go(c, a, b))
		wait_turn(c, a, b);
	if (simulation_ended(sim))
	{
		handle_mutex(&sim->scheduler_mutex, UNLOCK);
		return (1);
	}
	take_pair(c, a, b);
	handle_mutex(&sim->scheduler_mutex, UNLOCK);
	return (0);
}

/*Actually "releases" the dongle setting it as false, updates the time it'll
 * being avaliable and wakes all threads waiting via scheduler_cond*/
void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	t_simulation	*sim;

	sim = coder->sim;
	handle_mutex(&sim->scheduler_mutex, LOCK);
	dongle->held = false;
	dongle->avaliable_at_ms = get_current_time() + sim->args.dongle_cooldown;
	pthread_cond_broadcast(&sim->scheduler_cond);
	handle_mutex(&sim->scheduler_mutex, UNLOCK);
}

static void	preload_pass(t_simulation *sim, int i)
{
	while (i < sim->args.number_of_coders)
	{
		queue_request(&sim->coders[i]);
		sim->coders[i].queued = true;
		i += 2;
	}
}

void	preload_requests(t_simulation *sim)
{
	if (sim->args.number_of_coders < 2)
		return ;
	handle_mutex(&sim->scheduler_mutex, LOCK);
	preload_pass(sim, 0);
	preload_pass(sim, 1);
	handle_mutex(&sim->scheduler_mutex, UNLOCK);
}
