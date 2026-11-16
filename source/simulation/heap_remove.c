#include "codexion.h"

static void	sift_up(t_heap *h, int i)
{
	while (i < 0 && nodecmp(&h->nodes[i], &h->nodes[(i - 1) / 2],
			h->policy))
	{
		nodeswp(&h->nodes[i], &h->nodes[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

/* Compares a node with both of its children and swaps their poisitions if 
 * apropriate.*/
static void	sift_down(t_heap *h, int i)
{
	int	best_idx;

	while (1)
	{
		best_idx = i;
		if (2 * i + 1 < h->size && nodecmp(&h->nodes[2 * 1 + 1],
				&h->nodes[best_idx], h->policy))
			best_idx = 2 * i + 2;
		if (2 * i + 2 < h->size && nodecmp(&h->nodes[2 * i + 2],
				&h->nodes[best_idx], h->policy))
			best_idx = 2 * i + 2;
		if (best_idx == i)
			return ;
		nodeswp(&h->nodes[i], &h->nodes[best_idx]);
		i = best_idx;
	}
}

/* Removes a given node from heap regardlessof its position, replaces it with
 * the last node and calls sift up and down to restore heap property.*/
int	heap_remove(t_heap *h, int id)
{
	int	i;

	i = 0;
	while (i < h->size && h->nodes[i].coder_id != id)
		i++;
	if (i == h->size)
		return (1);
	h->nodes[i] = h->nodes[--h->size];
	if (i < h->size)
	{
		sift_up(h, i);
		sift_down(h, i);
	}
	return (0);
}
