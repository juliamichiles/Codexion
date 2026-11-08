#include "codexion.h"

bool	get_bool(t_mtx *mutex, bool *value)
{
	bool	ret;

	handle_mutex(mutex, LOCK);
	ret = *value;
	handle_mutex(mutex, UNLOCK);
	return (ret);
}

void	set_bool(t_mtx *mutex, bool *dest, bool value)
{
	handle_mutex(mutex, LOCK);
	*dest = value;
	handle_mutex(mutex, UNLOCK);
}

llong	get_llong(t_mtx *mutex, llong *value)
{
	llong	ret;

	handle_mutex(mutex, LOCK);
	ret = *value;
	handle_mutex(mutex, UNLOCK);
	return (ret);
}

void	set_llong(t_mtx *mutex, llong *dest, llong value)
{
	handle_mutex(mutex, LOCK);
	*dest = value;
	handle_mutex(mutex, UNLOCK);
}

bool	simulation_ended(t_simulation *sim)
{
	return (get_bool(&sim->state_mutex, &sim->stopped));
}
