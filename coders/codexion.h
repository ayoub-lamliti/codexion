/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:15:43 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:15:46 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <sys/time.h>
# include <stddef.h>
# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>

typedef struct s_system	t_system;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct s_heap	t_heap;

struct s_heap
{
	t_coder	**waiting_list;
	int		size;
	int		capacity;
};

struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t	wait_queue;
	long long		last_time_cooldown;
	int				is_available;
	t_heap			*queue;
};

struct s_coder
{
	pthread_t		thread_id;
	pthread_mutex_t	lock;
	int				id;
	int				is_in_queue;
	long long		last_compile_start;
	int				number_of_compiles;
	long long		wait_start_time;
	t_dongle		*right_dongle;
	t_dongle		*left_dongle;
	t_system		*sys;
};

struct s_system
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	long long		start_time;
	int				stop_simulation;
	int				threads_created;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t	state;
	pthread_mutex_t	log;
	pthread_t		monitor;
};

/* utils.c */
long long	get_time_by_ms(void);
void		custom_usleep(long long time_ms, t_system *sys);
int			check_simulation_stopped(t_system *sys);

/* parser.c */
int			check_is_digit(char *arg);
int			parser_and_init(char **args, t_system *sys);

/* heap.c */
void		swap(t_heap *heap, int i, int p);
int			heap_push(t_heap *heap, t_coder *coder);
t_coder		*heap_pop(t_heap *heap);

/* logger.c */
void		logger(char *msg, t_coder *coder);

/* simulation.c */
void		assign_dongles(t_dongle **f, t_dongle **s, t_coder *c);
void		take_dongle(t_dongle *dongle, t_coder *coder);
void		drop_dongle(t_dongle *dongle);
void		*start_simulation(void *data);

/* monitor.c */
void		*monitor(void *data);

/* init.c */
int			init_allocs(t_system *sys);
void		init_coders(t_system *sys);
void		cleanup_system(t_system *sys);

#endif