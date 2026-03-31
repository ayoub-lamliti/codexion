/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 18:17:46 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/31 12:37:36 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

static int	parse_and_init(int argc, char **argv, t_system *sys)
{
	if (argc != 9 || parse_args(argv, sys))
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	if (init_system(sys))
	{
		printf("Error: Invalid malloc/mutex initialization\n");
		return (1);
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_system	sys;
	int			i;

	memset(&sys, 0, sizeof(t_system));
	if (parse_and_init(argc, argv, &sys))
		return (1);
	if (start_simulation(&sys))
		printf("Error: Thread creation failed\n");
	else
		check_death(&sys);
	i = -1;
	while (++i < sys.number_of_coders)
		pthread_join(sys.coders[i].thread_id, NULL);
	i = -1;
	while (++i < sys.number_of_coders)
		pthread_mutex_destroy(&sys.dongles[i].mutex);
	pthread_mutex_destroy(&sys.state);
	pthread_mutex_destroy(&sys.log);
	free(sys.dongles);
	free(sys.coders);
	return (0);
}
