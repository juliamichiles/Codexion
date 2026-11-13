#include "codexion.h"

void	init_coder(t_simulation *sim, t_coder *coder, int index)
{
	coder->id = index + 1;
	coder->state = WAITING_FOR_DONGLES;
	coder->left_dongle = NULL;
	coder->right_dongle = NULL;
	coder->last_compile_start = -1;
	coder->compile_count = 0;
	coder->sim = sim;
}

/* Ensures coder always attempts to grab lower id dongl first to prevent 
 * circular wait.*/
static int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first->id > second->id)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	//FIXME? Maybe change to if !acquire_dongle???
	if (acquire_dongles(coder, first, second))
		return (1);
	print_status(coder->sim, coder->id, TAKEN_DONGLE);
	print_status(coder->sim, coder->id, TAKEN_DONGLE);
	return (0);
}

static int	compile(t_coder *coder)
{
	t_simulation	*sim;
	t_llong			*lst_cmp;
	int				done;

	sim = coder->sim;
	lst_cmp = &coder->last_compile_start;
	set_llong(&sim->state_mutex, lst_cmp, get_current_time());
	print_status(sim, coder->id, COMPILING);
	ms_sleep(sim, sim->args.time_to_compile);
	handle_mutex(&sim->state_mutex, LOCK);
	coder->compile_count += 1;
	done = (coder->compile_count >= sim->args.number_of_compiles);
	if (done)
		sim->finished_coders++;
	handle_mutex(&sim->state_mutex, UNLOCK);
	return (done);
}

static void	*one_coder(t_coder *coder)
{
	print_status(coder->sim, coder->id, TAKEN_DONGLE);
	while (!simulation_ended(coder->sim))
		usleep(500);
	return (NULL);
}

void	*coder_routine(void *arg)
{
	t_simulation	*sim;
	t_coder			*coder;
	int				done;

	if (!arg)
		return (NULL);
	coder = (t_coder *)arg;
	sim = coder->sim;
	if (coder->left_dongle == coder->right_dongle)
		return (one_coder(coder));
	while (!simulation_ended(sim))
	{
		if (take_dongles(coder))
			break ;
		done = compile(coder);
		release_dongle(coder, coder->left_dongle);
		release_dongle(coder, coder->right_dongle);
		if (done)
			break ;
		print_status(sim, coder->id, DEBUGGING);
		ms_sleep(sim, sim->args.time_to_debug);
		print_status(sim, coder->id, REFACTORING);
		ms_sleep(sim, sim->args.time_to_refactor);
	}
	return (NULL);
}
