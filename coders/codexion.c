/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 18:17:46 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/27 18:42:13 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

int main(int argc, char **argv)
{
    static t_system *sys;
    if (argc != 9 || parse_args(argv, sys))
        return (1);
    return (0);
}