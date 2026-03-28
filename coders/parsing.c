/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:06:54 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/28 10:06:55 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

static int	check_is_digit(const char *str)
{
	if (!str || !*str)
		return (1);
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (1);
		str++;
	}
	return (0);
}

int	parse_args(char **argv, t_system *sys)
{
	char	**argv_og;

	argv_og = argv;
	while (*argv_og++ && (argv_og - argv) != 8)
		if (check_is_digit(*argv_og) || atoi(*argv_og) <= 0)
			return (1);
	if (strcmp(*argv_og, "fifo") != 0 && strcmp(*argv_og, "edf") != 0)
		return (1);
	sys->number_of_coders = atoi(argv[1]);
	sys->time_to_burnout = atoi(argv[2]);
	sys->time_to_compile = atoi(argv[3]);
	sys->time_to_debug = atoi(argv[4]);
	sys->time_to_refactor = atoi(argv[5]);
	sys->number_of_compiles_required = atoi(argv[6]);
	sys->dongle_cooldown = atoi(argv[7]);
	if (strcmp(*argv_og, "fifo") == 0)
		sys->scheduler = 0;
	else
		sys->scheduler = 1;
	return (0);
}
