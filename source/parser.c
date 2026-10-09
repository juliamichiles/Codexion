#include "codexion.h"

// Caller must pass av+1 and ac-1
int	parse_numbers(char *arg)
{
	// add some other safeguard against accessing out of bound memory?
	//returns converted number or -1 for invalid argument
	// currently accepts trailing wtspcs and one single +
	int dgt;
	int n;

	n = 0;
	dgt = 0;
	while (ft_isspace(*arg))
		arg++;
	if (*arg == '+')
		arg++; // I decided to accept + sign, hope that's ok...
	if (!ft_isnum(*arg))
		return -1;
	while(ft_isnum(*arg))
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

int	parse_scheduler(char *arg)
{
	//returns 1 for FIFO, 2 for EDF or 0 for invalid argument
	//currently does not accept trailing wtpcs chars and is case-sensitive
	//maybe find a way to ignore wtpcs without having to allocate memory
	if (!strcmp(arg, "fifo"))
		return (1);
	if (!strcmp(arg, "edf"))
		return (2);
	return (0);
}

int	validate_info(t_args *info)
{
	// return 1 on success or 0 on failure
	// I decided not to accept 0, so change here in case we decide to accept it
	if (info->number_of_coders < 1)
		return 0;
	if (info->time_to_burnout < 0)
                return 0;
	if (info->time_to_compile < 0)
		return 0;
	if (info->time_to_debug < 0)
                return 0;
	if (info->time_to_refactor < 0)
                return 0;
	if (info->number_of_compiles < 0)
                return 0;
	if (info->dongle_cooldown < 0)
                return 0;
	if (info->scheduler < 1)
                return 0;
	return (1);
}

t_args	*parse_args(char *args[])
{
	t_args	*info;

 	info = malloc(sizeof(t_args));
	if (!info)
		return NULL;
	info->number_of_coders = parse_numbers(args[0]);
	info->time_to_compile = parse_numbers(args[1]);
	info->time_to_burnout = parse_numbers(args[2]);
	info->time_to_debug = parse_numbers(args[3]);
	info->time_to_refactor = parse_numbers(args[4]);
	info->number_of_compiles = parse_numbers(args[5]);
	info->dongle_cooldown = parse_numbers(args[6]);
	info->scheduler = parse_scheduler(args[7]);
	if (!validate_info(info))
	{
		free(info);
		return NULL;
	}
	return (info);
}
