#include "codexion.h"
//FIXME: Split this in two files


/*Sleeps for ~0.5 milisecons at a time while the given time in miliseconds
 * hasn't been reached and the simulation hasn't ended*/
void    ms_sleep(t_simulation *sim, llong ms)
{
    llong   end;

    end = get_current_time() + ms;
    while (!simulation_ended(sim) && get_current_time() < end)
        usleep(500);
}
/* Ensures coder always attempts to grab lower id dongl first to prevent circular
 * wait.*/
static int take_dongles(t_coder *coder)
{
    t_dongle    *first;
    t_dongle    *second;

    first = coder->left_dongle;
    second = coder->right_dongle;
    if (first->id > second->id)
    {
        first = coder->right_dongle;
        second = coder->left_dongle;
    }
    if (acquire_dongle(coder, first))
        return (1);
    print_status(coder->sim, coder->id, TAKEN_DONGLE);
    if (acquire_dongle(coder, second))
    {
        release_dongle(coder, first);
        return(1);
    }
    print_status(coder->sim, coder->id, TAKEN_DONGLE);
    return (0);
}

static int  compile(t_coder *coder)
{
    t_simulation    *sim;
    int             done;

    sim = coder->sim;
    set_llong(&sim->state_mutex, &coder->last_compile_start, get_current_time());
    print_status(sim, coder->id, COMPILING);
    ms_sleep(sim, sim->args.time_to_compile);
    handle_mutex(&sim->state_mutex, LOCK);
    coder->compile_count += 1;
    done = (coder->compile_count >= sim->args.number_of_compiles);
    if (done)
        sim->finished_coders++;
    handle_mutex(&sim->state_mutex, UNLOCK);
    return (done);
}

static void *one_coder(t_coder *coder)
{
    print_status(coder->sim, coder->id, TAKEN_DONGLE);
    while (!simulation_ended(coder->sim))
        usleep(500);
    return (NULL);
}

void    *coder_routine(void *arg)
{
    t_coder         *coder;
    t_simulation    *sim;
    int             done;

    coder = (t_coder *)arg;
    sim = coder->sim;
    if (coder->left_dongle == coder->right_dongle)
        return (one_coder(coder));
    while (!simulation_ended(sim))
    {
        if (take_dongles(coder))
            break ;
        done = compile(coder);
        release_dongle(coder, coder->left_dongle);
        release_dongle(coder, coder->right_dongle);
        if (done)
            break;
        print_status(sim, coder->id, DEBUGGING);
        ms_sleep(sim, sim->args.time_to_debug);
        print_status(sim, coder->id, REFACTORING);
        ms_sleep(sim, sim->args.time_to_refactor);
    }
    return (NULL);
}
