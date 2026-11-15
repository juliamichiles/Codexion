#include "codexion.h"

static void	enqueue_pair(t_coder *c, t_dongle *a, t_dongle *b)
{
	t_llong	deadline;

	deadline = get_llong(&c->sim->state_mutex, &c->last_compile_start)
		+ c->sim->args.time_to_burnout;
	heap_push(a->heap, a->next_ticket++, deadline, c->id);
	heap_push(b->heap, b->next_ticket++, deadline, c->id);
}

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
	enqueue_pair(c, a, b);
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
