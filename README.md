_This project has been created as part of the 42 curriculum by juliatav_

## Description

**Codexion** is a concurrency simulation written in C with POSIX threads. `number_of_coders` coders sit in a circle with one USB dongle between each pair of neighbours. Each coder cycles through compiling, debugging and refactoring. Compiling requires holding both neighbouring dongles at once. A coder who does not start compiling within `time_to_burnout` ms of their last compile start (or of the simulation start) burns out and the simulation stops. It also stops when every coder has compiled `number_of_compiles_required` times.

Dongles are granted according to a scheduler (`fifo` or `edf`) backed by a hand-written priority queue (heap), and each dongle has a cooldown after being released.

## Instructions

**Compile:**

    make

**Run:**

    ./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
               time_to_refactor number_of_compiles_required dongle_cooldown scheduler

All times are in milliseconds. `scheduler` is `fifo` or `edf`.

**Example:**

    ./codexion 5 800 200 200 200 7 50 edf

Other rules: `make clean`, `make fclean` and `make re`. Builds with `cc -Wall -Wextra -Werror -pthread`.

## Blocking cases handled

- **Deadlock prevention:** each coder always takes the dongle with the lower id first, so a circular wait (one of Coffman's conditions) cannot form.
- **Starvation prevention:** each dongle has its own heap of waiting coders. With `fifo` the oldest request wins. With `edf` the coder with the earliest deadline (`last_compile_start + time_to_burnout`) wins, and arrival order breaks ties.
- **Cooldown handling:** a released dongle records `avaliable_at_ms`. The next coder in line sleeps with `pthread_cond_timedwait` until the cooldown has passed.
- **Precise burnout detection:** a separate monitor thread checks every coder about every 1 ms, well inside the 10 ms limit. It sets the stop flag, prints the burnout line, and wakes all sleeping threads.
- **Log serialization:** every message is printed under `log_mutex`. Once the simulation has stopped, only the burnout message may still be printed.
- **Single coder:** with only one dongle, the coder takes it, waits, and burns out.

## Thread synchronization mechanisms

- **`pthread_mutex_t` (one per dongle):** protects the dongle's `held` flag, cooldown timestamp and waiting heap. A coder pushes its request and checks "am I at the top of the heap, is the dongle free, has the cooldown passed" under this mutex, so two coders can never take the same dongle.
- **`pthread_cond_t` (one per dongle):** coders wait on it instead of busy-waiting. `release_dongle` and `wake_everyone` broadcast on it, and `pthread_cond_timedwait` handles the cooldown.
- **`state_mutex`:** protects the `stopped` flag, each coder's `last_compile_start` and `compile_count`, and `finished_coders`. All access goes through the `get_*`/`set_*` helpers or explicit lock/unlock, so the monitor never reads half-updated values.
- **`log_mutex`:** serializes output so lines never interleave.
- **Coder ↔ monitor communication:** the monitor reads coder state under `state_mutex`. To stop the simulation it sets `stopped` under that same mutex and then broadcasts on every dongle's condition variable, so coders blocked in `pthread_cond_wait` wake up, see the flag and exit.
- No custom event implementation is used.

## Resources

### POSIX threads
- https://medium.com/@techdhaba.training/posix-threads-pthreads-the-simplest-way-to-understand-real-multithreading-in-c-c2f591ab7a03
- PTHREADS(7) Linux Programmer's Manual - POSIX threads

### Dining philosophers problem
- https://medium.com/@jalal92/the-dining-philosophers-7157cc05315
- https://www.youtube.com/watch?v=zOpzGHwJ3MU

### Use of AI
