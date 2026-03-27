/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 16:37:26 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/27 19:07:40 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

static int check_is_digit(char *str)
{
    if (!str || !*str)
        return 0;
    while (*str)
    {
        if (!isdigit(*str))
            return (1);
        str++;
    }
    return (0);
}

int parse_args(char **argv, t_system *sys)
{
    char **argv_og = argv;
    while (*argv_og++ && (argv_og - argv) != 8)
        if (check_is_digit(*argv_og))
            return (1);
    if (!strcmp(*argv_og, "fifo") && !strcmp(*argv_og, "edf"))
        return (1);
    printf("all good!!\n");
}