#include "codexion.h"

static bool has_burned_out(t_simulation *sim, t_coder *coder)
{
    bool    burned;
    int     deadline;

    handle_mutex(&sim->state_mutex, LOCK);
    deadline = sim->args.time_to_burnout;
    burned = coder->compile_count < sim->args.number_of_compiles
        && get_elapsed_time(coder->last_compile_start) >= deadline;
    handle_mutex(&sim->state_mutex, UNLOCK);
    return (burned);
}

static bool all_finished(t_simulation *sim)
{
    bool    done;

    handle_mutex(&sim->state_mutex, LOCK);
    done = sim->finished_coders >= sim->args.number_of_coders;
    handle_mutex(&sim->state_mutex, UNLOCK);
    return (done);
}

/*First we set the flag first so no other message gets printed
 * than print the burnout line
 * only than we wake the sleeping threads*/
static void declare_burnout(t_simulation *sim, t_coder *coder)
{
    set_bool(&sim->state_mutex, &sim->stopped, true);
    print_status(sim, coder->id, BURNED_OUT);
    wake_everyone(sim);
}

void    *monitor_routine(void *arg)
{
    t_simulation    *sim;
    int             i;

    sim = (t_simulation *)arg;
    while (!simulation_ended(sim))
    {
        if (all_finished(sim))
        {
            stop_simulation(sim);
            return (NULL);
        }
        i = 0;
        while (i < sim->args.number_of_coders)
        {
            if (has_burned_out(sim, &sim->coders[i]))
            {
                declare_burnout(sim, &sim->coders[i]);
                return (NULL);
            }
            i++;
        }
        usleep(1000);
    }
    return (NULL);
}
