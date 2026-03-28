/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:07:00 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/28 17:13:58 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./codexion.h"

long long get_time_in_ms(void)
{
    struct timeval time;
    if (gettimeofday(&time, NULL) == -1)
        return (-1);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void *coder_routine(void *coder)
{
    t_coder *_coder = ((t_coder *)coder);
    if (_coder->id % 2 == 0)
        usleep(1000);



        
    return (NULL);
}

void check_death(t_system *sys){}

int start_simulation(t_system *sys)
{
    int i;

    i = 0;
    sys->start_time = get_time_in_ms();
    if (sys->start_time == -1)
        return (1);
    while (i < sys->number_of_coders)
    {
        sys->coders[i].last_compile_start = sys->start_time;
        if (pthread_create(&sys->coders[i].thread_id, NULL, coder_routine, &sys->coders[i]) != 0)
            return (1);
        i++;
    }
    return (0);
}