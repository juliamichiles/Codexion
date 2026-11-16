#include "codexion.h"

t_heap	*init_heap(int capacity, int policy)
{
	t_heap	*heap;

	if (capacity <= 0)
		return (NULL);
	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->nodes = malloc(sizeof(t_node) * capacity);
	if (!heap->nodes)
	{
		free(heap);
		return (NULL);
	}
	heap->capacity = capacity;
	heap->size = 0;
	heap->policy = policy;
	return (heap);
}

int	nodecmp(t_node *a, t_node *b, int policy)
{
	if (policy == 2)
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
	}
	return (a->arrival_time < b->arrival_time);
}

void	nodeswp(t_node *a, t_node *b)
{
	t_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

/* compare node with its parent and swap if necessary*/
void	heap_push(t_heap *heap, t_llong arrival_time, t_llong deadline, int id)
{
	int	policy;
	int	i;

	i = heap->size;
	if (heap->size >= heap->capacity)
		return ;
	policy = heap->policy;
	heap->nodes[i].coder_id = id;
	heap->nodes[i].arrival_time = arrival_time;
	heap->nodes[i].deadline = deadline;
	heap->size += 1;
	while (i > 0 && nodecmp(&heap->nodes[i], &heap->nodes[(i - 1) / 2], policy))
	{
		nodeswp(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}
