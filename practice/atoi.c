#include <unistd.h>

int	ft_atoi(char *s)
{
	int	sign;
	int	nbr;

	nbr = 0;
	sign = 1;
	if (!s || !(*s))
		return 0;
	while (*s == 32 || (*s >= 8 && *s <= 13))
		s++;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -sign;
		s++;
	}
	while (*s && (*s >= '0' && *s <= '9'))
	{
		nbr = (nbr * 10) + (*s - 48);
		s++;
	}
	return (nbr * sign);
}

#include <stdlib.h>
#include <stdio.h>
int	main(int ac, char **av)
{

	if (ac != 2)
		printf("Usage: ./atoi.c <string>\n");
	else
		printf("OG: %d\nFT: %d\n", atoi(av[1]), ft_atoi(av[1]));
	return 0;
}
