#include "codexion.h"

void	*cleanup(void *memory, void *message)
{
	if (message)
		p_error((char *)message);
	free(memory);
	return (NULL);
}

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->nodes);
	free(heap);
}

void	dongle_cleanup(t_dongle **dongles, int size)
{
	int i;

	if (!dongles || !*dongles)
		return;
	i = 0;
	while (i < size)
	{
        heap_destroy((*dongles)[i].heap);
		handle_mutex(&(*dongles)[i].mutex, DESTROY);
		i++;
	}
	free(*dongles);
	*dongles = NULL;
}
