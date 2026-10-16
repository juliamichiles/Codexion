#include "codexion.h"

// - Copy the parsed arguments into sim->args.
// - Allocate the array of coders and the array of dongles.
// - Initialize the shared mutexes and condition variable.
// - Initialize each coder and dongle.
// - Connect each coder to its left and right dongles in a circle.

int	init_simulation(t_simulation *sim, const t_args *args)
{
	if (!sim || !args)
		return (0);
	sim->args = *args;
	
}

int	destroy_simulation(t_simulation *sim)
{}
