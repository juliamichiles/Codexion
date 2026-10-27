#include "codexion.h"

t_heap *init_heap(int capacity, int policy)
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

static int	node_comp(t_node *a, t_node *b, int policy)
{
	// return choosen node instead?
	if (policy == 2)
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
		return (a->arrival_time < b->arrival_time);
	}
	else if (policy == 1) 
		return (a->arrival_time < b->arrival_time);
}

static void	swap_nodes(t_node *a, t_node *b)
{
	t_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	heap_push(t_heap *heap, llong arrival_time, llong deadline, int id)
{
    int policy;
	int	i;

	i = heap->size;
	if (heap->size >= heap->capacity)
		return ;
    policy = heap->policy;
	heap->nodes[i].coder_id = id;
	heap->nodes[i].arrival_time = arrival_time;
	heap->nodes[i].deadline = deadline;
	heap->size += 1;
    while (i > 0 && node_comp(&heap->nodes[i], &heap->nodes[(i - 1) / 2], policy))
	{
        // compare node with its parent and swap if necessary
        swap_nodes(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);
        i = (i - 1) / 2; // goes back to parent's idx
	}
}

int heap_pop(t_heap *heap, t_node *out_node)
{
    int smallest;
    int policy;
    int right;
    int left;
    int i;

    if (heap->size <= 0)
        return (1);
    i = 0;
    policy = heap->policy;
    *out_node = heap->nodes[0]; //first node
    heap->nodes[0] = heap->nodes[--heap->size];
    while (1)
    {
        smallest = i;
        left = 2 * i + 1;
        right = 2 * i + 2;
        if (left < heap->size 
                && node_comp(&heap->nodes[left], &heap->nodes[smallest], policy))
            smallest = left;
        if (right < heap->size 
                && node_comp(&heap->nodes[right], &heap->nodes[smallest], policy))
            smallest = right;
        if (smallest == i)
            break;
        swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
        i = smallest;
    }
    return (0);
}
