/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 18:17:46 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/28 10:05:58 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

int	main(int argc, char **argv)
{
	t_system	sys;

	memset(&sys, 0, sizeof(t_system));
	if (argc != 9 || parse_args(argv, &sys))
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	if (init_system(&sys))
    {
        printf("Error: Invalid malloc initialization\n");    
		return (1);
    }
	return (0);
}
