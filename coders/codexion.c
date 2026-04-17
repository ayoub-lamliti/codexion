/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:08:33 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:16:03 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	create_threads(t_system *sys)
{
	int	i;

	i = -1;
	while (++i < sys->number_of_coders)
	{
		if (pthread_create(&sys->coders[i].thread_id, NULL,
				start_simulation, &sys->coders[i]) == 0)
			sys->threads_created++;
		else
		{
			printf("\033[31mError\033[0m: Failed to create thread %d\n", i);
			pthread_mutex_lock(&sys->state);
			sys->stop_simulation = 1;
			pthread_mutex_unlock(&sys->state);
			break ;
		}
	}
	if (sys->threads_created == sys->number_of_coders)
		pthread_create(&sys->monitor, NULL, monitor, sys);
}

static void	join_threads(t_system *sys)
{
	int	i;

	if (sys->threads_created == sys->number_of_coders)
		pthread_join(sys->monitor, NULL);
	i = -1;
	while (++i < sys->threads_created)
		pthread_join(sys->coders[i].thread_id, NULL);
}

int	main(int argc, char **argv)
{
	t_system	sys;

	if (argc != 9)
	{
		printf("\033[31mError\033[0m: invalid number of arguments\n");
		return (1);
	}
	if (parser_and_init(argv, &sys))
		return (1);
	if (init_allocs(&sys))
		return (1);
	init_coders(&sys);
	create_threads(&sys);
	join_threads(&sys);
	cleanup_system(&sys);
	return (0);
}
