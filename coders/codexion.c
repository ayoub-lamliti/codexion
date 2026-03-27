/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 18:17:46 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/27 20:23:52 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

int main(int argc, char **argv)
{
    t_system *sys = malloc(sizeof(t_system) * int argv[1]);
    if (argc != 9 || parse_args(argv, sys))
        return (1);
    return (0);
}