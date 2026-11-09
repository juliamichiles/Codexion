#include "codexion.h"

void	print_status(t_simulation *sim, int coder_id, t_coder_state state)
{
	t_llong	time_stamp;

	handle_mutex(&sim->log_mutex, LOCK);
	if (simulation_ended(sim) && state != BURNED_OUT)
	{
		handle_mutex(&sim->log_mutex, UNLOCK);
		return ;
	}
	time_stamp = get_elapsed_time(sim->start_time_ms);
	if (state == TAKEN_DONGLE)
		printf("%lld %d has taken a dongle\n", time_stamp, coder_id);
	else if (state == COMPILING)
		printf("%lld %d is compiling\n", time_stamp, coder_id);
	else if (state == DEBUGGING)
		printf("%lld %d is debugging\n", time_stamp, coder_id);
	else if (state == REFACTORING)
		printf("%lld %d is refactoring\n", time_stamp, coder_id);
	else if (state == BURNED_OUT)
		printf("%lld %d burned out\n", time_stamp, coder_id);
	handle_mutex(&sim->log_mutex, UNLOCK);
}

static bool	has_burned_out(t_simulation *sim, t_coder *coder)
{
	bool	burned;
	int		deadline;

	handle_mutex(&sim->state_mutex, LOCK);
	deadline = sim->args.time_to_burnout;
	burned = coder->compile_count < sim->args.number_of_compiles
		&& get_elapsed_time(coder->last_compile_start) >= deadline;
	handle_mutex(&sim->state_mutex, UNLOCK);
	return (burned);
}

static bool	all_finished(t_simulation *sim)
{
	bool	done;

	handle_mutex(&sim->state_mutex, LOCK);
	done = sim->finished_coders >= sim->args.number_of_coders;
	handle_mutex(&sim->state_mutex, UNLOCK);
	return (done);
}

/*First we set the flag first so no other message gets printed
 * than print the burnout line
 * only than we wake the sleeping threads*/
static void	declare_burnout(t_simulation *sim, t_coder *coder)
{
	set_bool(&sim->state_mutex, &sim->stopped, true);
	print_status(sim, coder->id, BURNED_OUT);
	wake_everyone(sim);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				i;

	sim = (t_simulation *)arg;
	while (!simulation_ended(sim))
	{
		if (all_finished(sim))
		{
			stop_simulation(sim);
			return (NULL);
		}
		i = 0;
		while (i < sim->args.number_of_coders)
		{
			if (has_burned_out(sim, &sim->coders[i]))
			{
				declare_burnout(sim, &sim->coders[i]);
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
