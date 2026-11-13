#include "codexion.h"

/* Coder is on top of this dongle's heap, dongle is free and cooled down.*/
bool	is_ready(t_dongle *d, int id)
{
	if (d->heap->size == 0 || d->heap->nodes[0].coder_id != id)
		return (false);
	return ((bool)(!d->held && d->avaliable_at_ms <= get_current_time()));
}

/* If the dongle is not being held puts that coder's thread to sleep until the dongle has cooled down otherwise puts it to sleep until a broadcast*/
void	wait_dongle(t_dongle *dongle)
{
	struct timespec	ts;

	if (!dongle->held && dongle->avaliable_at_ms > get_current_time())
	{
		ts.tv_sec = dongle->avaliable_at_ms / 1000;
		ts.tv_nsec = (dongle->avaliable_at_ms % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	else
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

/*Actually "releases" the dongle setting it as false, updates the time it'll 
 * being avaliable and wakes all threads waiting on this dongle's condition 
 * variable.*/
void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	handle_mutex(&dongle->mutex, LOCK);
	dongle->held = false;
	dongle->avaliable_at_ms = get_current_time()
		+ coder->sim->args.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	handle_mutex(&dongle->mutex, UNLOCK);
}

void	lock_pair(t_dongle *a, t_dongle *b)
{
	handle_mutex(&a->mutex, LOCK);
	handle_mutex(&b->mutex, LOCK);
}

void	unlock_pair(t_dongle *a, t_dongle *b)
{
	handle_mutex(&a->mutex, UNLOCK);
	handle_mutex(&b->mutex, UNLOCK);
}
