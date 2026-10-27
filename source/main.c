#include "codexion.h"

void	print_args(t_args *info)
{
	printf("number_of_coders: %d\n", info->number_of_coders);
	printf("time_to_burnout: %d\n", info->time_to_burnout);
	printf("time_to_compile: %d\n", info->time_to_compile);
	printf("time_to_debug: %d\n", info->time_to_debug);
	printf("time_to_refactor: %d\n", info->time_to_refactor);
	printf("number_of_compiles: %d\n", info->number_of_compiles);
	printf("dongle_cooldown: %d\n", info->dongle_cooldown);
	printf("scheduler: %d\n", info->scheduler);
}

void	print_dongles(t_dongle *dongles, int size)
{
	int	i = 0;
	while (i < size)
	{
		printf("| Dongle %d - avaliable at %lld ms |\n", 
				dongles[i].id, dongles[i].avaliable_at_ms);
		i++;
	}
}

void	print_coders(t_coder *coders, int size)
{
	int	i = 0;
	while (i < size)
	{
		printf("---\n");
		printf("[Coder %d]\n", coders[i].id);
		printf(" Current State: %d\n", coders[i].state);
		printf(" Left dongle's id: %d\n", coders[i].left_dongle->id);
		printf(" Right dongle's id: %d\n", coders[i].right_dongle->id);
		printf(" Last compile at: %lld ms\n", coders[i].last_compile_start);
		printf(" Compile count: %d\n", coders[i].compile_count);
		printf(" Simulation addr.: %p\n", coders[i].sim);
		i++;
	}
}

void	print_simulation(t_simulation *sim)
{
	printf("=== SIMULATION ===\n");
	printf("[ARGS]\n");
	print_args(&sim->args);
	printf("\n");
	printf("[DONGLES]\n");
	if (!sim->dongles)
		printf("[NULL]\n");
	else
		print_dongles(sim->dongles, sim->args.number_of_coders);
	printf("\n");
	printf("[CODERS]\n");
	if (!sim->coders)
		printf("[NULL]\n");
	else
		print_coders(sim->coders, sim->args.number_of_coders);
	printf("\n");
	if (sim->stopped)
		printf("[Simulation stopped: YES]\n");
	else
		printf("[Simulation stopped: NO]\n");
	printf("[Start time: %lld]\n", sim->start_time_ms);
	printf("[Finished coders so far: %d]\n", sim->finished_coders);
}

int	main(int ac, char *av[])
{
	t_args		*info;
	t_simulation	*sim;

	if (ac != 9)
	{
		p_error(
			"Usage: ./codexion number_of_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"number_of_compiles_required dongle_cooldown scheduler");
		return (1);
	}
	info = parse_args(av + 1);
	if (!info)
		return (1);
	// print_args(info);
	// TODO: write a helper to handle allocating the sim. at simulation.c
	sim = malloc(sizeof(t_simulation) * 1);
	if (!sim)
	{
		free(info);
		p_error("Simulation memory allocation failed.");
		return (1);
	}
	if (init_simulation(sim, info) != 0)
	{
		free(sim);
		free(info);
		return (1);
	}
	print_simulation(sim);
	destroy_simulation(sim);
	free(sim);
	free(info);
	return (0);
}
