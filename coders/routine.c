/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 12:12:36 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/31 12:33:29 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

static void	assign_dongles(t_dongle **first, t_dongle **second, t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
	else
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
}

static void	custom_usleep(long long time_in_ms, t_system *sys)
{
	long long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < time_in_ms)
	{
		pthread_mutex_lock(&sys->state);
		if (sys->simulation_stop == 1)
		{
			pthread_mutex_unlock(&sys->state);
			break ;
		}
		pthread_mutex_unlock(&sys->state);
		usleep(500);
	}
}

static int	handle_single_coder(t_coder *coder, t_dongle *first_dongle)
{
	if (coder->sys->number_of_coders == 1)
	{
		pthread_mutex_lock(&first_dongle->mutex);
		logger(coder->sys, coder->id, "has taken a dongle");
		while (1)
		{
			pthread_mutex_lock(&coder->sys->state);
			if (coder->sys->simulation_stop)
			{
				pthread_mutex_unlock(&coder->sys->state);
				break ;
			}
			pthread_mutex_unlock(&coder->sys->state);
			usleep(1000);
		}
		pthread_mutex_unlock(&first_dongle->mutex);
		return (1);
	}
	return (0);
}

static void	execute_coder_cycle(
	t_coder *coder,
	t_dongle *first,
	t_dongle *second
)
{
	pthread_mutex_lock(&first->mutex);
	logger(coder->sys, coder->id, "has taken a dongle");
	pthread_mutex_lock(&second->mutex);
	logger(coder->sys, coder->id, "has taken a dongle");
	pthread_mutex_lock(&coder->sys->state);
	coder->last_compile_start = get_time_in_ms();
	coder->compile_count += 1;
	pthread_mutex_unlock(&coder->sys->state);
	logger(coder->sys, coder->id, "is compiling");
	custom_usleep(coder->sys->time_to_compile, coder->sys);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	logger(coder->sys, coder->id, "is debugging");
	custom_usleep(coder->sys->time_to_debug, coder->sys);
	logger(coder->sys, coder->id, "is refactoring");
}

void	*coder_routine(void *coder)
{
	t_dongle	*first_dongle;
	t_dongle	*second_dongle;
	t_coder		*_coder;

	_coder = ((t_coder *)coder);
	assign_dongles(&first_dongle, &second_dongle, _coder);
	if (handle_single_coder(_coder, first_dongle))
		return (NULL);
	while (1)
	{
		pthread_mutex_lock(&_coder->sys->state);
		if (_coder->sys->simulation_stop)
		{
			pthread_mutex_unlock(&_coder->sys->state);
			break ;
		}
		pthread_mutex_unlock(&_coder->sys->state);
		execute_coder_cycle(coder, first_dongle, second_dongle);
	}
	return (NULL);
}
