/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:09:20 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:28:36 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

#include "codexion.h"

void	assign_dongles(t_dongle **f, t_dongle **s, t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		*f = coder->right_dongle;
		*s = coder->left_dongle;
	}
	else
	{
		*f = coder->left_dongle;
		*s = coder->right_dongle;
	}
}

void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	long long	remaining;

	pthread_mutex_lock(&dongle->lock);
	coder->wait_start_time = get_time_by_ms();
	heap_push(dongle->queue, coder);
	while (dongle->is_available == 0 || dongle->queue->waiting_list[0] != coder)
	{
		pthread_cond_wait(&dongle->wait_queue, &dongle->lock);
		if (check_simulation_stopped(coder->sys))
		{
			coder->is_in_queue = 0;
			pthread_mutex_unlock(&dongle->lock);
			return ;
		}
	}
	heap_pop(dongle->queue);
	dongle->is_available = 0;
	remaining = coder->sys->dongle_cooldown - (get_time_by_ms()
			- dongle->last_time_cooldown);
	pthread_mutex_unlock(&dongle->lock);
	if (remaining > 0)
		custom_usleep(remaining, coder->sys);
}

void	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	dongle->last_time_cooldown = get_time_by_ms();
	pthread_cond_broadcast(&dongle->wait_queue);
	pthread_mutex_unlock(&dongle->lock);
}

static int	execute_cycle(t_coder *coder, t_dongle *first, t_dongle *second)
{
	take_dongle(first, coder);
	logger("has taken a dongle", coder);
	if (coder->sys->number_of_coders == 1)
	{
		custom_usleep(coder->sys->time_to_burnout + 100, coder->sys);
		return (1);
	}
	take_dongle(second, coder);
	logger("has taken a dongle", coder);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_start = get_time_by_ms();
	coder->number_of_compiles++;
	pthread_mutex_unlock(&coder->lock);
	logger("is compiling", coder);
	custom_usleep(coder->sys->time_to_compile, coder->sys);
	drop_dongle(first);
	drop_dongle(second);
	logger("is debugging", coder);
	custom_usleep(coder->sys->time_to_debug, coder->sys);
	logger("is refactoring", coder);
	custom_usleep(coder->sys->time_to_refactor, coder->sys);
	return (0);
}

void	*start_simulation(void *data)
{
	t_coder		*coder;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)data;
	assign_dongles(&first, &second, coder);
	if (coder->id % 2 == 0)
		custom_usleep(10, coder->sys);
	while (!check_simulation_stopped(coder->sys))
	{
		if (execute_cycle(coder, first, second))
			break ;
	}
	return (NULL);
}
