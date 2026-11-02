#include "codexion.h"

bool is_top(t_dongle *d, int id)
{
    return ((bool)(d->heap->size > 0 && d->heap->nodes[0].coder_id == id));
}

void wait_dongle(t_dongle *dongle, int id)
{
    struct timespec ts;

    if (is_top(dongle, id) && !dongle->held)
    {
        ts.tv_sec = dongle->avaliable_at_ms / 1000;
        ts.tv_nsec = (dongle->avaliable_at_ms % 1000) * 1000000;
        pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
    }
    else
        pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

int  acquire_dongle(t_coder *coder, t_dongle *dongle)
{
    t_simulation    *sim;
    llong           deadline;
    t_node          trash;

    sim = coder->sim;
    deadline = get_llong(&sim->state_mutex, &coder->last_compile_start)
        + sim->args.time_to_burnout;
    handle_mutex(&dongle->mutex, LOCK);
    heap_push(dongle->heap, dongle->next_ticket++, deadline, coder->id);
    while (!simulation_ended(sim) && !(is_top(dongle, coder->id) 
                && !dongle->held && dongle->avaliable_at_ms <= get_current_time()))
        wait_dongle(dongle, coder->id);
    if (simulation_ended(sim))
    {
        handle_mutex(&dongle->mutex, UNLOCK);
        return (1);
    }
    heap_pop(dongle->heap, &trash);
    dongle->held = true;
    handle_mutex(&dongle->mutex, UNLOCK);
    return (0);
}

void release_dongle(t_coder *coder, t_dongle *dongle)
{
    handle_mutex(&dongle->mutex, LOCK);
    dongle->held = false;
    dongle->avaliable_at_ms = get_current_time() 
        + coder->sim->args.dongle_cooldown;
    pthread_cond_broadcast(&dongle->cond);
    handle_mutex(&dongle->mutex, UNLOCK);
}
