#include "codexion.h"

// TODO: Add error hanlers using errno?
// TODO: Add helpers to allocate memory for dongles and coders and handle failure
// freeing memory if malloc fails


int	handle_mutex(t_mtx *mutex, t_op code)
{
	// CALLER MUST CHECK:
	// if (handle_mutex(&sim->log_mutex, INIT) != 0)
	// {
    	//	p_error("Failed to initialize log mutex.");
    	//	return (1);
	// }
	if (code == LOCK)
		return (pthread_mutex_lock(mutex));
	else if (code == UNLOCK)
		return (pthread_mutex_unlock(mutex));
	else if (code == INIT)
		return (pthread_mutex_init(mutex, NULL));
	else if (code == DESTROY)
		return (pthread_mutex_destroy(mutex));
	return (EINVAL);
		// p_error("Invalid operation code for mutex.");
}


/*
static void	handle_thread_error(int status, t_op code)
{
	if (status == 0)
		return;
	if (status == EAGAIN)
		p_error("No resoureces to create another thread");
	else if (status == EPERM)
		p_error("The caller does not have appropriate permission.");
	else if (status == EINVAL && code == CREATE)
		p_error("The value specified by attr is invalid.");
	else if (status == EINVAL && (code == JOIN || code == DETACH))
		p_error("The value specified by thread is not joinable.");
	else if (status == ESRCH)
		p_error("No thread could be found corresponding to that"
				"specified by the given thread ID, thread.");
	else if (status == EDEADLK)
		p_error("A deadlock was detected or the value of thread specifies"
				"the calling thread.")
}
*/

int	handle_thread(pthread_t *thread, void *(*f)(void *),
		void *data, t_op code)
{
	if (code == CREATE)
		 return (pthread_create(thread, NULL, f, data));
	else if (code == JOIN)
		 return (pthread_join(*thread, NULL));
	else if (code == DETACH)
		 return (pthread_detach(*thread));
	return (EINVAL);
	// p_error("Invalid operation code for thread.");
}
