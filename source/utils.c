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
