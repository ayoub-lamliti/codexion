/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:09:47 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:28:25 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	broadcast_all(t_system *sys)
{
	int	j;

	j = -1;
	while (++j < sys->number_of_coders)
		pthread_cond_broadcast(&sys->dongles[j].wait_queue);
}

static int	check_burnout(t_system *sys, int i)
{
	if (get_time_by_ms() - sys->coders[i].last_compile_start
		> sys->time_to_burnout)
	{
		pthread_mutex_unlock(&sys->coders[i].lock);
		pthread_mutex_lock(&sys->log);
		pthread_mutex_lock(&sys->state);
		sys->stop_simulation = 1;
		pthread_mutex_unlock(&sys->state);
		printf("%lld %d burned out\n", get_time_by_ms() - sys->start_time,
			sys->coders[i].id);
		pthread_mutex_unlock(&sys->log);
		broadcast_all(sys);
		return (1);
	}
	return (0);
}

static int	check_simulation_status(t_system *sys)
{
	int	i;
	int	finished;

	i = -1;
	finished = 0;
	while (++i < sys->number_of_coders)
	{
		pthread_mutex_lock(&sys->coders[i].lock);
		if (check_burnout(sys, i))
			return (1);
		if (sys->coders[i].number_of_compiles
			>= sys->number_of_compiles_required)
			finished++;
		pthread_mutex_unlock(&sys->coders[i].lock);
	}
	if (finished == sys->number_of_coders)
	{
		pthread_mutex_lock(&sys->state);
		sys->stop_simulation = 1;
		pthread_mutex_unlock(&sys->state);
		broadcast_all(sys);
		return (1);
	}
	return (0);
}

void	*monitor(void *data)
{
	t_system	*sys;

	sys = (t_system *)data;
	while (1)
	{
		if (check_simulation_status(sys))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
