/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:09:59 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:16:15 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_is_digit(char *arg)
{
	int		i;
	long	num;

	i = 0;
	num = 0;
	if (!arg || !*arg)
		return (-1);
	if (arg[i] == '+')
		i++;
	while (arg[i])
	{
		if (!(arg[i] >= '0' && arg[i] <= '9'))
			return (-1);
		if (num > (INT_MAX - (arg[i] - '0')) / 10)
			return (-1);
		num = num * 10 + (arg[i] - '0');
		i++;
	}
	if (num == 0)
		return (-1);
	return ((int)num);
}

static void	print_parser_error(int index)
{
	char	*msg[8];

	msg[0] = "\033[31mError\033[0m: number of coders must be positive\n";
	msg[1] = "\033[31mError\033[0m: time to burnout must be positive\n";
	msg[2] = "\033[31mError\033[0m: time to compile must be positive\n";
	msg[3] = "\033[31mError\033[0m: time to debug must be positive\n";
	msg[4] = "\033[31mError\033[0m: time to refactor must be positive\n";
	msg[5] = "\033[31mError\033[0m: number of compiles must be positive\n";
	msg[6] = "\033[31mError\033[0m: dongle cooldown must be positive\n";
	msg[7] = "\033[31mError\033[0m: scheduler must be either fifo or edf\n";
	printf("%s", msg[index]);
}

static void	assign_sys_values(t_system *sys, int *values, char *sched)
{
	sys->number_of_coders = values[1];
	sys->time_to_burnout = values[2];
	sys->time_to_compile = values[3];
	sys->time_to_debug = values[4];
	sys->time_to_refactor = values[5];
	sys->number_of_compiles_required = values[6];
	sys->dongle_cooldown = values[7];
	if (!strcmp(sched, "fifo"))
		sys->scheduler = 0;
	else
		sys->scheduler = 1;
}

int	parser_and_init(char **args, t_system *sys)
{
	int	i;
	int	values[8];

	i = 1;
	while (args[i] && i < 8)
	{
		values[i] = check_is_digit(args[i]);
		if (values[i] == -1)
		{
			print_parser_error(i - 1);
			return (1);
		}
		i++;
	}
	if (strcmp(args[i], "fifo") != 0 && strcmp(args[i], "edf") != 0)
	{
		print_parser_error(7);
		return (1);
	}
	assign_sys_values(sys, values, args[i]);
	return (0);
}
