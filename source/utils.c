#include "codexion.h"

int	ft_isspace(char c)
{
	if (c == 32 || (c >= 8 && c <= 13))
		return (1);
	return (0);
}

int	ft_isnum(char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

void	handle_mutex(t_mtx *mutex, t_op code)
{
	if (code == LOCK)
		pthread_mutex_lock(mutex);
	else if (code == UNLOCK)
		pthread_mutex_unlock(mutex);
	else if (code == INIT)
		pthread_mutex_init(mutex, NULL);
	else if (code == DESTROY)
		pthread_mutex_destroy(mutex);
	else
		fprintf(stderr, "Invalid operation code for mutex.\n")
}
