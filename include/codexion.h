#ifndef CODEXION_H
# define CODEXION_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

typedef struct s_args
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles;
	int	dongle_cooldown;
	int	scheduler;
}		t_args;

// REMOVE LATER!!
void    print_args(t_args *info);

// utils:
int	ft_isspace(char c);
int	ft_isnum(char c);

//parser:
t_args	*parse_args(char *args[]);

#endif
