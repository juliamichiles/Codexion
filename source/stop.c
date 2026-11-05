#include "codexion.h"

void    wake_everyone(t_simulation *sim)
{
    int i;

    i = 0;
    while (i < sim->args.number_of_coders)
    {
        handle_mutex(&sim->dongles[i].mutex, LOCK);
        pthread_cond_broadcast(&sim->dongles[i].cond);
        handle_mutex(&sim->dongles[i].mutex, UNLOCK);
        i++;
    }
}

void    stop_simulation(t_simulation *sim)
{
    set_bool(&sim->state_mutex, &sim->stopped, true);
    wake_everyone(sim);
}
