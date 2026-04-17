/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:08:49 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:16:09 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_allocs(t_system *sys)
{
	sys->dongles = malloc(sizeof(t_dongle) * sys->number_of_coders);
	sys->coders = malloc(sizeof(t_coder) * sys->number_of_coders);
	if (!sys->dongles || !sys->coders)
	{
		printf("\033[31mError\033[0m: malloc failed\n");
		return (1);
	}
	memset(sys->dongles, 0, sys->number_of_coders * sizeof(t_dongle));
	memset(sys->coders, 0, sys->number_of_coders * sizeof(t_coder));
	pthread_mutex_init(&sys->state, NULL);
	pthread_mutex_init(&sys->log, NULL);
	sys->start_time = get_time_by_ms();
	sys->stop_simulation = 0;
	sys->threads_created = 0;
	return (0);
}

void	init_coders(t_system *sys)
{
	int	i;

	i = -1;
	while (++i < sys->number_of_coders)
	{
		sys->coders[i].id = i + 1;
		sys->coders[i].sys = sys;
		sys->coders[i].left_dongle = &sys->dongles[i];
		sys->coders[i].right_dongle = &sys->dongles[(i + 1)
			% sys->number_of_coders];
		sys->coders[i].last_compile_start = sys->start_time;
		sys->dongles[i].is_available = 1;
		sys->dongles[i].queue = malloc(sizeof(t_heap));
		sys->dongles[i].queue->capacity = sys->number_of_coders;
		sys->dongles[i].queue->size = 0;
		sys->dongles[i].queue->waiting_list = malloc(sizeof(t_coder *)
				* sys->number_of_coders);
		memset(sys->dongles[i].queue->waiting_list, 0,
			sys->number_of_coders * sizeof(t_coder *));
		pthread_cond_init(&sys->dongles[i].wait_queue, NULL);
		pthread_mutex_init(&sys->dongles[i].lock, NULL);
		pthread_mutex_init(&sys->coders[i].lock, NULL);
	}
}

void	cleanup_system(t_system *sys)
{
	int	i;

	i = -1;
	while (++i < sys->number_of_coders)
	{
		pthread_mutex_destroy(&sys->coders[i].lock);
		pthread_mutex_destroy(&sys->dongles[i].lock);
		pthread_cond_destroy(&sys->dongles[i].wait_queue);
		free(sys->dongles[i].queue->waiting_list);
		free(sys->dongles[i].queue);
	}
	pthread_mutex_destroy(&sys->state);
	pthread_mutex_destroy(&sys->log);
	free(sys->coders);
	free(sys->dongles);
}
