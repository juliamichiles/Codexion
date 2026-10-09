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

int	main(int ac, char *av[])
{
	if (ac != 9)
	{
		//FIXME: Can I print whatever error message I want?
		//FIXME: Not even sure I like this one...
		printf("Invalid format!\nExpected arguments:\n");
		printf("number_of_coders time_to_burnout time_to_compile");
		printf(" time_to_debug time_to_refactor number_of_compiles_");
		printf("required dongle_cooldown scheduler");
		return (1);
	}

	t_args	*info;

	info = parse_args(av + 1);
	if (!info)
	{
		printf("Parsing error\n");
		return (1);
	}
	print_args(info);
	free(info);
	return (0);
	

}
