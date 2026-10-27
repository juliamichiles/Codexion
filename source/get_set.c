#include "codexion.h"

//FIXME: Not sure those types actually match our current data structures
// Might need to typecast int into llongs to use these
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
	return (get_bool(&sim->state_mutex, (bool *)&sim->stopped));
}
