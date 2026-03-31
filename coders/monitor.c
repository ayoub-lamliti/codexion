/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 11:45:14 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/31 11:45:48 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

static int	check_death_helper(t_system *sys, int index, int *finished_coders)
{
	long long	curr_time;

	pthread_mutex_lock(&sys->state);
	curr_time = get_time_in_ms() - sys->coders[index].last_compile_start;
	if (sys->time_to_burnout <= curr_time)
	{
		sys->simulation_stop = 1;
		pthread_mutex_unlock(&sys->state);
		pthread_mutex_lock(&sys->log);
		curr_time = get_time_in_ms() - sys->start_time;
		printf("%lld %d %s\n", curr_time, sys->coders[index].id, "burned out");
		pthread_mutex_unlock(&sys->log);
		return (1);
	}
	if (sys->coders[index].compile_count >= sys->number_of_compiles_required)
		(*finished_coders)++;
	pthread_mutex_unlock(&sys->state);
	return (0);
}

void	check_death(t_system *sys)
{
	int	i;
	int	finished_coders;

	while (1)
	{
		i = 0;
		finished_coders = 0;
		while (i < sys->number_of_coders)
		{
			if (check_death_helper(sys, i, &finished_coders))
				return ;
			i++;
		}
		if (finished_coders == sys->number_of_coders)
		{
			pthread_mutex_lock(&sys->state);
			sys->simulation_stop = 1;
			pthread_mutex_unlock(&sys->state);
			return ;
		}
		usleep(1000);
	}
}
