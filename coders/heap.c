/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamliti <alamliti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/17 20:09:03 by alamliti          #+#    #+#             */
/*   Updated: 2026/04/17 20:28:02 by alamliti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap(t_heap *heap, int i, int p)
{
	t_coder	*temp;

	if (!heap || i >= heap->size || p >= heap->size)
		return ;
	temp = heap->waiting_list[i];
	heap->waiting_list[i] = heap->waiting_list[p];
	heap->waiting_list[p] = temp;
}

static int	compare_deadlines(t_coder *c1, t_coder *c2, int is_edf)
{
	long long	d1;
	long long	d2;

	if (is_edf)
	{
		d1 = c1->last_compile_start;
		d2 = c2->last_compile_start;
	}
	else
	{
		d1 = c1->wait_start_time;
		d2 = c2->wait_start_time;
	}
	return (d1 < d2);
}

int	heap_push(t_heap *heap, t_coder *coder)
{
	int	i;
	int	p;

	if (heap->capacity == heap->size || coder->is_in_queue)
		return (heap->capacity == heap->size);
	heap->waiting_list[heap->size] = coder;
	coder->is_in_queue = 1;
	heap->size++;
	i = heap->size - 1;
	while (i > 0)
	{
		p = (i - 1) / 2;
		if (compare_deadlines(heap->waiting_list[i], heap->waiting_list[p],
				coder->sys->scheduler))
		{
			swap(heap, i, p);
			i = p;
		}
		else
			break ;
	}
	return (0);
}

static int	get_smallest_child(t_heap *h, int i, int is_edf)
{
	int	left;
	int	right;
	int	smallest;

	left = 2 * i + 1;
	right = 2 * i + 2;
	smallest = left;
	if (left >= h->size)
		return (-1);
	if (right < h->size)
	{
		if (compare_deadlines(h->waiting_list[right],
				h->waiting_list[left], is_edf))
			smallest = right;
	}
	return (smallest);
}

t_coder	*heap_pop(t_heap *h)
{
	t_coder	*coder;
	int		i;
	int		small;

	if (!h->size)
		return (NULL);
	swap(h, 0, h->size - 1);
	coder = h->waiting_list[--h->size];
	coder->is_in_queue = 0;
	h->waiting_list[h->size] = NULL;
	i = 0;
	while (i < h->size)
	{
		small = get_smallest_child(h, i, coder->sys->scheduler);
		if (small == -1 || !compare_deadlines(h->waiting_list[small],
				h->waiting_list[i], coder->sys->scheduler))
			break ;
		swap(h, i, small);
		i = small;
	}
	return (coder);
}
