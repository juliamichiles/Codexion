#include "codexion.h"

int	ft_isspace(char c)
{
	if (c == 32 || (c >= 8 && c <= 13))
		return (1);
	return (0);
}

int	ft_isnum(char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

void	p_error(char *message)
{
	fprintf(stderr, "%s\n", message);
}

long long	get_current_time(void)
{
	struct timeval	tv;
	// that's ok with the norm right?
	
	if (gettimeofday(&tv, NULL))
	{
		p_error("gettimeofday failed.");
		return (-1); // or 0??
	}
	return ((tv.tv_ssec * 1000LL) + (tv.tv_usec / 1000LL));
}

long long get_elapsed_time(long long start_time)
{
	return (get_current_time() - start_time);
}
