/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliatav <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 18:45:14 by juliatav          #+#    #+#             */
/*   Updated: 2026/11/20 18:45:17 by juliatav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

t_llong	get_elapsed_time(t_llong start_time)
{
	return (get_current_time() - start_time);
}

void	*cleanup(void *memory, void *message)
{
	if (message)
		p_error((char *)message);
	free(memory);
	return (NULL);
}
