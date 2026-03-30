/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayoub-lec <ayoub-lec@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:07:00 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/30 18:55:05 by ayoub-lec        ###   ########.fr       */
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

static void print_msg(t_system *sys, int id, char *msg)
{
    long long current_time;

    pthread_mutex_lock(&sys->log);

    pthread_mutex_lock(&sys->state);
    if (sys->simulation_stop == 1)
    {
        pthread_mutex_unlock(&sys->state);
        pthread_mutex_unlock(&sys->log);
        return;
    }
    pthread_mutex_unlock(&sys->state);

    current_time = get_time_in_ms() - sys->start_time;

    printf("%lld %d %s\n", current_time, id, msg);

    pthread_mutex_unlock(&sys->log);
}

void *coder_routine(void *coder)
{
    t_coder *_coder = ((t_coder *)coder);
    t_dongle *first_dongle;
    t_dongle *second_dongle;

    if (_coder->id % 2 == 0)
    {
        first_dongle = _coder->right_dongle;
        second_dongle = _coder->left_dongle;
    }
    else
    {
        first_dongle = _coder->left_dongle;
        second_dongle = _coder->right_dongle;
    }
    if (_coder->sys->number_of_coders == 1)
    {
        pthread_mutex_lock(&first_dongle->mutex);
        print_msg(_coder->sys, _coder->id, "has taken a dongle");
        while (1)
        {
            pthread_mutex_lock(&_coder->sys->state);
            if (_coder->sys->simulation_stop)
            {
                pthread_mutex_unlock(&_coder->sys->state);
                break;
            }
            pthread_mutex_unlock(&_coder->sys->state);
            usleep(1000);
        }
        pthread_mutex_unlock(&first_dongle->mutex);
        return (NULL);
    }
    while (1)
    {
        pthread_mutex_lock(&_coder->sys->state);
        if (_coder->sys->simulation_stop)
        {
            pthread_mutex_unlock(&_coder->sys->state);
            break;
        }
        pthread_mutex_unlock(&_coder->sys->state);

        pthread_mutex_lock(&first_dongle->mutex);
        print_msg(_coder->sys, _coder->id, "has taken a dongle");

        pthread_mutex_lock(&second_dongle->mutex);
        print_msg(_coder->sys, _coder->id, "has taken a dongle");

        pthread_mutex_lock(&_coder->sys->state);
        _coder->last_compile_start = get_time_in_ms();
        _coder->compile_count += 1;
        pthread_mutex_unlock(&_coder->sys->state);
        print_msg(_coder->sys, _coder->id, "is compiling");

        usleep(_coder->sys->time_to_compile * 1000);

        pthread_mutex_unlock(&second_dongle->mutex);
        pthread_mutex_unlock(&first_dongle->mutex);

        print_msg(_coder->sys, _coder->id, "is debugging");

        usleep(_coder->sys->time_to_debug * 1000);

        print_msg(_coder->sys, _coder->id, "is refactoring");
    }

    return (NULL);
}

void check_death(t_system *sys)
{
    long long current_time;
    int i;
    int finished_coders;

    while (1)
    {
        i = 0;
        finished_coders = 0;
        while (i < sys->number_of_coders)
        {
            pthread_mutex_lock(&sys->state);
            current_time = get_time_in_ms() - sys->coders[i].last_compile_start;
            if (sys->time_to_burnout <= current_time)
            {
                sys->simulation_stop = 1;
                pthread_mutex_unlock(&sys->state);
                pthread_mutex_lock(&sys->log);
                current_time = get_time_in_ms() - sys->start_time;
                printf("%lld %d %s\n", current_time, sys->coders[i].id, "burned out");
                pthread_mutex_unlock(&sys->log);
                return;
            }
            if (sys->coders[i].compile_count >= sys->number_of_compiles_required)
                finished_coders++;
            pthread_mutex_unlock(&sys->state);
            i++;
        }
        if (finished_coders == sys->number_of_coders)
        {
            pthread_mutex_lock(&sys->state);
            sys->simulation_stop = 1;
            pthread_mutex_unlock(&sys->state);
            return;
        }
        usleep(1000);
    }
}

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