#include "codexion.h"

int	handle_mutex(t_mtx *mutex, t_op code)
{
		if (code == LOCK)
		return (pthread_mutex_lock(mutex));
	else if (code == UNLOCK)
		return (pthread_mutex_unlock(mutex));
	else if (code == INIT)
		return (pthread_mutex_init(mutex, NULL));
	else if (code == DESTROY)
		return (pthread_mutex_destroy(mutex));
	return (EINVAL);
}

int	handle_thread(pthread_t *thread, void *(*f)(void *),
		void *data, t_op code)
{
	if (code == CREATE)
		 return (pthread_create(thread, NULL, f, data));
	else if (code == JOIN)
		 return (pthread_join(*thread, NULL));
	return (EINVAL);
}

llong   get_current_time(void)
{
	struct timeval  tv;

	if (gettimeofday(&tv, NULL))
	{
		p_error("gettimeofday failed.");
		return (-1);
	}
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}
