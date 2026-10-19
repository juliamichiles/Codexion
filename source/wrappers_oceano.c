#include "codexion.h"

// TODO: Add error hanlers using errno?
// TODO: Add helpers to allocate memory for dongles and coders and handle failure
// freeing memory if malloc fails

static void	handle_mutex_error(int status, t_op code)
{
	if (status == 0)
		return ;
	if (status == EINVAL && (code == LOCK || code == UNLOCK))
		p_error("The value specified by mutex is invalid.");
	else if (status == EINVAL && code == INIT)
		p_error("The value specified by attr is invalid.");
	else if (status == EDEADLK)
		// FIXME: Actually print the whole error message
		p_error("A deadlock would occur if the thread blocke waiting"
				"for mutex.");
	else if (status == EPERM)
		p_error("Current thread does not hold a lock on mutex.");
	else if (status == ENOMEM)
		p_error("Cannot allocate enough memory to create another mutex.");
	else if (status == EBUSY)
		p_error("Mutex is locked.");

}

void	handle_mutex(t_mtx *mutex, t_op code)
{
	if (code == LOCK)
		handle_mutex_error(pthread_mutex_lock(mutex), code);
	else if (code == UNLOCK)
		handle_mutex_error(pthread_mutex_unlock(mutex), code);
	else if (code == INIT)
		handle_mutex_error(pthread_mutex_init(mutex, NULL), code);
	else if (code == DESTROY)
		handle_mutex_error(pthread_mutex_destroy(mutex), code);
	else
		p_error("Invalid operation code for mutex.");
}

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


void	handle_thread(pthread_t *thread, void *(*f)(void *),
		void *data, t_op code)
{
	if (code == CREATE)
		handle_thread_error(pthread_create(thread, NULL, f, data), code);
	else if (code == JOIN)
		handle_thread_error(pthread_join(*thread, NULL), code);
	else if (code == DETACH)
		handle_thread_error(pthread_detach(*thread), code);
	else
		p_error("Invalid operation code for thread.");
}
