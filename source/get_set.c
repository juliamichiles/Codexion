#include "codexion.h"

//FIXME: Not sure those types actually match our current data structures
// Might need to typecast int into long longs to use these
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

long long	get_long(t_mtx *mutex, long long *value)
{
	long long	ret;

	handle_mutex(mutex, LOCK);
	ret = *value;
	handle_mutex(mutex, UNLOCK);
	return (ret);
}

void	set_long(t_mtx *mutex, long long *dest, long long value)
{
	handle_mutex(mutex, LOCK);
	*dest = value;
	handle_mutex(mutex, UNLOCK);
}
//TODO: Add a simulation_finished function? for readability returns a bool 
// if t_simulation->stopped is True/False (1/0)
