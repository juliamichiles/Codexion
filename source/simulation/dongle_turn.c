/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_turn.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliatav <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 18:42:39 by juliatav          #+#    #+#             */
/*   Updated: 2026/11/20 18:42:40 by juliatav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	is_next(t_dongle *d, int id)
{
	return (d->heap->size > 0 && d->heap->nodes[0].coder_id == id);
}

/* Strict arbitration: coder must be first in line on BOTH dongles and
 * both must be free (cooldown is checked separately).*/
static bool	has_turn(t_dongle *a, t_dongle *b, int id)
{
	return (!a->held && !b->held && is_next(a, id) && is_next(b, id));
}

bool	can_go(t_coder *c, t_dongle *a, t_dongle *b)
{
	t_llong	now;

	now = get_current_time();
	return (has_turn(a, b, c->id)
		&& a->avaliable_at_ms <= now && b->avaliable_at_ms <= now);
}

/* Scheduler mutex held. Timed wait if only the cooldown is in the way,
 * otherwise sleep until a release/stop broadcast.*/
void	wait_turn(t_coder *c, t_dongle *a, t_dongle *b)
{
	struct timespec	ts;
	t_llong			at;

	if (!has_turn(a, b, c->id))
	{
		pthread_cond_wait(&c->sim->scheduler_cond,
			&c->sim->scheduler_mutex);
		return ;
	}
	at = a->avaliable_at_ms;
	if (b->avaliable_at_ms > at)
		at = b->avaliable_at_ms;
	ts.tv_sec = at / 1000;
	ts.tv_nsec = (at % 1000) * 1000000;
	pthread_cond_timedwait(&c->sim->scheduler_cond,
		&c->sim->scheduler_mutex, &ts);
}

void	queue_request(t_coder *c)
{
	t_dongle	*l;
	t_dongle	*r;
	t_llong		deadline;

	l = c->left_dongle;
	r = c->right_dongle;
	deadline = get_llong(&c->sim->state_mutex, &c->last_compile_start)
		+ c->sim->args.time_to_burnout;
	heap_push(l->heap, l->next_ticket++, deadline, c->id);
	heap_push(r->heap, r->next_ticket++, deadline, c->id);
}
