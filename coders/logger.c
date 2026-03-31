/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 11:35:28 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/31 12:22:09 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

void	logger(t_system *sys, int id, char *msg)
{
	long long	current_time;

	pthread_mutex_lock(&sys->log);
	pthread_mutex_lock(&sys->state);
	if (sys->simulation_stop == 1)
	{
		pthread_mutex_unlock(&sys->state);
		pthread_mutex_unlock(&sys->log);
		return ;
	}
	pthread_mutex_unlock(&sys->state);
	current_time = get_time_in_ms() - sys->start_time;
	printf("%lld %d %s\n", current_time, id, msg);
	pthread_mutex_unlock(&sys->log);
}
