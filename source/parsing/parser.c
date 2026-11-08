#include "codexion.h"

int	parse_numbers(char *arg)
{
	int	dgt;
	int	n;

	n = 0;
	dgt = 0;
	while (ft_isspace(*arg))
		arg++;
	if (*arg == '+')
		arg++;
	if (!ft_isnum(*arg))
		return (-1);
	while (ft_isnum(*arg))
	{
		dgt = *arg - '0';
		if (n > INT_MAX / 10 || (n == INT_MAX / 10 && dgt > INT_MAX % 10))
			return (-1);
		n = n * 10 + dgt;
		arg++;
	}
	while (ft_isspace(*arg))
		arg++;
	if (*arg)
		return (-1);
	return (n);
}

/*returns 1 for FIFO, 2 for EDF or 0 for invalid argument*/
int	parse_scheduler(char *arg)
{
	if (!strcmp(arg, "fifo"))
		return (1);
	if (!strcmp(arg, "edf"))
		return (2);
	return (0);
}

static int	print_error(char *type, char *culprit)
{
	fprintf(stderr, "Invalid argument '%s': %s\n", type, culprit);
	return (0);
}

int	validate_info(t_args *info, char *args[])
{
	int	valid;

	valid = 1;
	if (info->number_of_coders < 0)
		valid = print_error("number of coders", args[0]);
	if (info->time_to_burnout < 0)
		valid = print_error("time to burnout", args[1]);
	if (info->time_to_compile < 0)
		valid = print_error("time to compile", args[2]);
	if (info->time_to_debug < 0)
		valid = print_error("time to time_to_debug", args[3]);
	if (info->time_to_refactor < 0)
		valid = print_error("time to refactor", args[4]);
	if (info->number_of_compiles < 0)
		valid = print_error("number of compiles required", args[5]);
	if (info->dongle_cooldown < 0)
		valid = print_error("dongle_cooldown", args[6]);
	if (info->scheduler < 1)
		valid = print_error("scheduler", args[7]);
	return (valid);
}

t_args	*parse_args(char *args[])
{
	t_args	*info;

	info = malloc(sizeof(t_args));
	if (!info)
		return (NULL);
	info->number_of_coders = parse_numbers(args[0]);
	info->time_to_burnout = parse_numbers(args[1]);
	info->time_to_compile = parse_numbers(args[2]);
	info->time_to_debug = parse_numbers(args[3]);
	info->time_to_refactor = parse_numbers(args[4]);
	info->number_of_compiles = parse_numbers(args[5]);
	info->dongle_cooldown = parse_numbers(args[6]);
	info->scheduler = parse_scheduler(args[7]);
	if (!validate_info(info, args))
	{
		free(info);
		return (NULL);
	}
	return (info);
}
