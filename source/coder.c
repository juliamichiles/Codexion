#include "codexion.h"
//FIXME: Split this in two files

void    ms_sleep(t_simulation *sim, llong ms)
{
    llong   end;

    end = get_current_time() + ms;
    while (!simulation_ended(sim) && get_current_time() < end)
        usleep(500);
}

static bool is_top(t_dongle dngl, int id)
{
    return ((bool)(dngl->heap->size > 0 && d->heap->nodes[0].coder_id == id));
}

static llong    ready_time(t_coder *coder)
{
    llong   left;
    llong   right;

    left = coder->left_dongle->avaliable_at_ms;
    right = coder->right_dongle->avaliable_at_ms;
    if (left > right)
        return (left);
    return (right);
}


