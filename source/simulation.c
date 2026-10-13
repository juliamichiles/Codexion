#include "codexion.h"

int	init_simulation(t_simulation *sim, const t_args *args)
{
	if (!sim || !args)
		return (0);
	sim->args = *args;
	
}

int	destroy_simulation(t_simulation *sim)
{}
