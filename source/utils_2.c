#include "codexion.h"

void    *cleanup(void *memory, void *message)
{
    if (message)
        p_error((char *)message);
    free(memory);
    return (NULL);
}
