/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:10:12 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:16:21 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_by_ms(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		return (-1);
	return (((long long)time.tv_sec * 1000) + (time.tv_usec / 1000));
}

int	check_simulation_stopped(t_system *sys)
{
	int	status;

	pthread_mutex_lock(&sys->state);
	status = sys->stop_simulation;
	pthread_mutex_unlock(&sys->state);
	return (status);
}

void	custom_usleep(long long time_ms, t_system *sys)
{
	long long	start_time;
	long long	elapsed;

	start_time = get_time_by_ms();
	while (1)
	{
		if (check_simulation_stopped(sys))
			break ;
		elapsed = get_time_by_ms() - start_time;
		if (elapsed >= time_ms)
			break ;
		if (time_ms - elapsed > 1)
			usleep(500);
		else
			usleep(50);
	}
}

void	logger(char *msg, t_coder *coder)
{
	pthread_mutex_lock(&coder->sys->log);
	if (check_simulation_stopped(coder->sys))
	{
		pthread_mutex_unlock(&coder->sys->log);
		return ;
	}
	printf("%lld %d %s\n", get_time_by_ms() - coder->sys->start_time,
		coder->id, msg);
	pthread_mutex_unlock(&coder->sys->log);
}
