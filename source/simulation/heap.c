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

static int	nodecmp(t_node *a, t_node *b, int policy)
{
	if (policy == 2)
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
	}
	return (a->arrival_time < b->arrival_time);
}

static void	nodeswp(t_node *a, t_node *b)
{
	t_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

/* compare node with its parent and swap if necessary*/
void	heap_push(t_heap *heap, llong arrival_time, llong deadline, int id)
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

int	heap_pop(t_heap *heap, t_node *out_node)
{
	t_heap_var	var;

	if (heap->size <= 0)
		return (1);
	var.i = 0;
	var.policy = heap->policy;
	*out_node = heap->nodes[0];
	heap->nodes[0] = heap->nodes[--heap->size];
	while (1)
	{
		var.smallest = var.i;
		var.left = 2 * var.i + 1;
		var.right = 2 * var.i + 2;
		if (var.left < heap->size && nodecmp(&heap->nodes[var.left],
				&heap->nodes[var.smallest], var.policy))
			var.smallest = var.left;
		if (var.right < heap->size && nodecmp(&heap->nodes[var.right],
				&heap->nodes[var.smallest], var.policy))
			var.smallest = var.right;
		if (var.smallest == var.i)
			break ;
		nodeswp(&heap->nodes[var.i], &heap->nodes[var.smallest]);
		var.i = var.smallest;
	}
	return (0);
}
