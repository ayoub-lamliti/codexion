/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 16:31:02 by alamliti          #+#    #+#             */
/*   Updated: 2026/03/27 19:06:00 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
typedef struct s_system t_system;

typedef struct s_dongle
{
    pthread_mutex_t mutex;
    long long available_at;
} t_dongle;

typedef struct s_coder
{
    int id;
    pthread_t thread_id;
    int compile_count;
    long long last_compile_start;
    t_dongle *left_dongle;
    t_dongle *right_dongle;
    t_system *sys;
} t_coder;

typedef struct s_system
{
    int number_of_coders;
    int time_to_compile;
    int time_to_burnout;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    int scheduler;

    long long start_time;
    int simulation_stop;
    pthread_mutex_t state;
    pthread_mutex_t log;

    t_dongle *dongles;
    t_coder *coders;
} t_system;

int parse_args(char **argv, t_system *sys);

#endif