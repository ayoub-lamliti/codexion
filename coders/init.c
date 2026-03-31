/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:06:43 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/31 12:25:14 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

static void	init_helper(t_system *sys)
{
	int	i;

	i = 0;
	while (i < sys->number_of_coders)
	{
		pthread_mutex_init(&sys->dongles[i].mutex, NULL);
		sys->dongles[i].available_at = 0;
		sys->coders[i].id = i + 1;
		sys->coders[i].compile_count = 0;
		sys->coders[i].sys = sys;
		sys->coders[i].left_dongle = &sys->dongles[i];
		sys->coders[i].right_dongle = &sys->dongles[
			(i + 1) % sys->number_of_coders];
		sys->coders[i].last_compile_start = 0;
		i++;
	}
}

int	init_system(t_system *sys)
{
	sys->coders = malloc(sizeof(t_coder) * sys->number_of_coders);
	if (sys->coders == NULL)
		return (1);
	sys->dongles = malloc(sizeof(t_dongle) * sys->number_of_coders);
	if (sys->dongles == NULL)
	{
		free(sys->coders);
		return (1);
	}
	pthread_mutex_init(&sys->state, NULL);
	pthread_mutex_init(&sys->log, NULL);
	init_helper(sys);
	return (0);
}
