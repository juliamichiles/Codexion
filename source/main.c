/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliatav <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 18:41:23 by juliatav          #+#    #+#             */
/*   Updated: 2026/11/20 18:41:24 by juliatav         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	codexion(t_args *info)
{
	t_simulation	*sim;

	if (info->number_of_coders == 0 || info->number_of_compiles == 0)
		return ;
	sim = malloc(sizeof(t_simulation) * 1);
	if (!sim)
	{
		p_error("Simulation memory allocation failed.");
		return ;
	}
	if (init_simulation(sim, info) != 0)
	{
		free(sim);
		return ;
	}
	run_simulation(sim);
	destroy_simulation(sim);
	free(sim);
	return ;
}

int	main(int ac, char *av[])
{
	t_args		*info;

	if (ac != 9)
	{
		p_error("Error: Expected exactly 8 arguments.");
		return (1);
	}
	info = parse_args(av + 1);
	if (!info)
		return (1);
	codexion(info);
	free(info);
	return (0);
}
