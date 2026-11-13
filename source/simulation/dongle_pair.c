#include "codexion.h"

static void	enqueue_pair(t_coder *c, t_dongle *a, t_dongle *b)
{
	t_llong	deadline;

	deadline = get_llong(&c->sim->state_mutex, &c->last_compile_start)
		+ c->sim->args.time_to_burnout;
	lock_pair(a, b);
	heap_push(a->heap, a->next_ticket++, deadline, c->id);
	heap_push(b->heap, b->next_ticket++, deadline, c->id);
	unlock_pair(a, b);
}

static void	take_pair(t_dongle *a, t_dongle *b)
{
	t_node	temp;

	heap_pop(a->heap, &temp);
	heap_pop(b->heap, &temp);
	a->held = true;
	b->held = true;
	unlock_pair(a, b);
}

/*unlocks whichever mutex is avaliable and waits for the other tobe avaliable,
 * once they are, locks both pairs. If none are avaliable returns 0*/
static int	wait_for_pair(t_coder *c, t_dongle *a, t_dongle *b)
{
	t_dongle	*blocked;
	t_dongle	*other;
	
	// What if both are avaliable???
	if (!is_ready(a, c->id))
	{
		blocked = a;
		other = b;
	}
	else if (!is_ready(b, c->id))
	{
		blocked = b;
		other = a;
	}
	else
		return (0);
	handle_mutex(&other->mutex, UNLOCK);
	wait_dongle(blocked);
	handle_mutex(&blocked->mutex, UNLOCK);
	lock_pair(a, b);
	return (1);
}

int	acquire_dongles(t_coder *c, t_dongle *a, t_dongle *b)
{
	enqueue_pair(c, a, b);
	lock_pair(a, b);
	while (!simulation_ended(c->sim))
	{
		if (!wait_for_pair(c, a, b))
		{
			take_pair(a, b);
			return (0);
		}
	}		
	unlock_pair(a, b);
	return (1);

}
