#include "codexion.h"

bool is_top(t_dongle *d, int id)
{
    return ((bool)(d->heap->size > 0 && d->heap->nodes[0].coder_id == id));
}

/* If the dongle is not being held and the given coder is "next in line"
 * puts that coder's thread to sleep for a given amount of time (until dongle 
 * is avaliable) otherwise puts it to sleep indefinitely*/
void wait_dongle(t_dongle *dongle, int coder_id)
{
    struct timespec ts;

    if (is_top(dongle, coder_id) && !dongle->held)
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
    handle_mutex(&dongle->mutex, LOCK); //protects all t_dongle's variables
    heap_push(dongle->heap, dongle->next_ticket++, deadline, coder->id);
    // checks if coder has priority, dongle is free and cooldown has passed,
    // if any of those is false the coder waits (loop bellow)
    while (!simulation_ended(sim) && !(is_top(dongle, coder->id) 
                && !dongle->held && dongle->avaliable_at_ms <= get_current_time()))
        wait_dongle(dongle, coder->id);
    if (simulation_ended(sim))
    {
        handle_mutex(&dongle->mutex, UNLOCK);
        return (1);
    }
    // now we finally acquire the dongle:
    heap_pop(dongle->heap, &trash); // remove coder from heap
    dongle->held = true; //set it as held
    handle_mutex(&dongle->mutex, UNLOCK); // unlock mutex
    return (0);
}

/*Actually "releases" the dongle setting it as free, updates the time it'll being
 * avaliable and wakes all threads waiting on this dongle's condition var.*/
void release_dongle(t_coder *coder, t_dongle *dongle)
{
    handle_mutex(&dongle->mutex, LOCK);
    dongle->held = false;
    dongle->avaliable_at_ms = get_current_time() 
        + coder->sim->args.dongle_cooldown;
    pthread_cond_broadcast(&dongle->cond);
    handle_mutex(&dongle->mutex, UNLOCK);
}
