#include "codexion.c"

// TODO: Add error hanlers using errno?

void	handle_mutex(pthread_mutex_t *mutex, t_op code)
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
		fprintf(stderr, "Invalid operation code for mutex.\n");
}

void	handle_thread(pthread_t *thread, void *(*f)(void *),
		void *data, t_op code)
{
	// FIXME: Does NOT handle errors!
	if (code == CREATE)
		pthread_create(thread, NULL, f, data);
	else if (code == JOIN)
		pthread_join(*thread, NULL);
	else if (code == DETACH)
		pthread_detach(*thread);
	else
		fprintf(stderr, "Invalid operation code for thread.\n");
}
