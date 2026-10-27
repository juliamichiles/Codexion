#include "codexion.h"

// TODO: Should I actually have a separate enum state for logging? 
// I don't think it's necessary, but maybe I'm wrong who knows...
void	print_status(t_simulation *sim, int coder_id, t_coder_state state)
{
	llong	time_stamp;

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
