#include "codexion.h"

//FIXME: Not sure those types actually match our current data structures
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

long	get_long(t_mtx *mutex, long *value)
{
	long	ret;

	handle_mutex(mutex, LOCK);
	ret = *value;
	handle_mutex(mutex, UNLOCK);
	return (ret);
}

void	set_long(t_mtx *mutex, long *dest, long value)
{
	handle_mutex(mutex, LOCK);
	*dest = value;
	handle_mutex(mutex, UNLOCK);
}
//TODO: Add a simulation_finished function? for readability returns a bool 
// if t_simulation->stopped is True/False (1/0)
