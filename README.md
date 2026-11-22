_This project has been created as part of the 42 curriculum by juliatav_

## Description

**Codexion** is a concurrency simulation inspired by the classic philosophers problem, written in C with POSIX threads. Here `number_of_coders` coders sit in a circle with one USB dongle between each pair of neighbours. Each coder cycles through compiling, debugging and refactoring. 

Compiling requires holding both neighbouring dongles at once. A coder who doesn't compile  for `time_to_burnout` ms burns out, after compiling, dongles must cooldown for `time_to_cooldown` ms. The simulation ends when every coder has compiled `number_of_compiles_required` times or when a coder burns out.

If two or more coders request the same dongle, it should be granted according to a scheduler (`fifo` or `edf`) backed by a hand-written priority queue (heap).

## Instructions

**Compile:**

    make

**Run:**

    ./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
               time_to_refactor number_of_compiles_required dongle_cooldown scheduler

All times are in milliseconds. `scheduler` is `fifo` or `edf`.

**Example:**

    ./codexion 5 900 200 200 200 7 50 edf

Other rules: `make clean`, `make fclean` and `make re`. Builds with `cc -Wall -Wextra -Werror -pthread`.
**Example output:**

```
    $ ./codexion 2 10000 200 300 500 2 800 fifo
    1 1 has taken a dongle
    2 1 has taken a dongle
    2 1 is compiling
    202 1 is debugging
    502 1 is refactoring
    1026 2 has taken a dongle
    1026 2 has taken a dongle
    1026 2 is compiling
    1226 2 is debugging
    1526 2 is refactoring
    2050 1 has taken a dongle
    2050 1 has taken a dongle
    2051 1 is compiling
    3058 2 has taken a dongle
    3058 2 has taken a dongle
    3058 2 is compiling
```

## Project's structure:

```mermaid
flowchart LR
    SIM["t_simulation"]
    ARGS["t_args<br/>timings, coder count,<br/>compile quota, scheduler"]
    CODERS["t_coder array"]
    DONGLES["t_dongle array"]

    SIM --> ARGS
    SIM --> CODERS
    SIM --> DONGLES

    CODERS --> C1["Coder 1"]
    CODERS --> C2["Coder 2"]
    CODERS --> CN["... Coder N"]

    DONGLES --> D1["Dongle 1"]
    DONGLES --> D2["Dongle 2"]
    DONGLES --> DN["... Dongle N"]

    C1 -->|"left_dongle / right_dongle"| D1
    C1 -->|"neighbor"| D2
    C2 -->|"neighbors"| D2
    C2 -->|"neighbor"| DN
    CN -->|"neighbors; circular arrangement"| DN
    CN -->|"neighbor"| D1

    C1 -. "sim pointer" .-> SIM
    C2 -. "sim pointer" .-> SIM
    CN -. "sim pointer" .-> SIM

    D1 --> H1["Per-dongle t_heap"]
    D2 --> H2["Per-dongle t_heap"]
    DN --> HN["Per-dongle t_heap"]
```
## Blocking cases handled

- **Deadlock prevention (atomic pair acquisition):** a coder can acquire its two neighbouring dongles only when both are free and the coder is first in both dongles’ queues. The pair is marked held together under the scheduler mutex, so a coder never holds one dongle while waiting for the other. `take_dongles` orders the dongles by ID, but acquisition is atomic rather than one-at-a-time.
- **Scheduler ordering and deterministic startup:** each dongle has a priority queue of waiting coders. With `fifo`, the earliest queued request wins. With `edf`, the earliest deadline (`last_compile_start + time_to_burnout`) wins, with queue arrival order breaking ties. Before coder threads start, the program preloads requests in a fixed order—odd-numbered coders, then even-numbered coders—so the initial queue order does not depend on thread startup timing. Each coder must be first in both queues to acquire its pair.
- **Cooldown handling:** releasing a dongle records its next available time. A coder that is first in both queues and has both dongles free waits with `pthread_cond_timedwait` until the later cooldown expires; otherwise, it waits for a scheduler-condition broadcast.
- **Burnout detection:** a separate monitor checks coders approximately every 0,5 ms. When a coder’s time since its last compile start exceeds `time_to_burnout`, the monitor stops the simulation, prints the burnout line, and wakes threads waiting on the scheduler condition variable. Detection timing can also depend on thread scheduling.
- **Log serialization:** every status message is printed under `log_mutex`. Status printing checks whether the simulation has stopped and suppresses regular messages if it has; the monitor prints the burnout message.
- **Single coder:** with one coder, there is only one dongle, so the coder cannot acquire a pair. It attepts to take the dongle, but never gets it since, the implementation only allows to acquire both at once, then waits for the monitor to detect burnout.

## Thread synchronization mechanisms

- **`pthread_mutex_t` (one per dongle):** protects the dongle's `held` flag, cooldown timestamp and waiting heap. A coder pushes its request and checks "am I at the top of the heap, is the dongle free, has the cooldown passed" under this mutex, so two coders can never take the same dongle.
- **`pthread_cond_t` (one per dongle):** coders wait on it instead of busy-waiting. `release_dongle` and `wake_everyone` broadcast on it, and `pthread_cond_timedwait` handles the cooldown.
- **`state_mutex`:** protects the `stopped` flag, each coder's `last_compile_start` and `compile_count`, and `finished_coders`. All access goes through the `get_*`/`set_*` helpers or explicit lock/unlock, so the monitor never reads half-updated values.
- **`log_mutex`:** serializes output so lines never interleave.
- **Coder ↔ monitor communication:** the monitor reads coder state under `state_mutex`. To stop the simulation it sets `stopped` under that same mutex and then broadcasts on every dongle's condition variable, so coders blocked in `pthread_cond_wait` wake up, see the flag and exit.
- **Wrappers around the POSIX API:** Raw `pthread_*` calls are kept out of the main code and centralised in wrapper functions. Mutex operations go through `handle_mutex(mutex, LOCK | UNLOCK | INIT | DESTROY)`, while thread operations use `handle_thread(thread, routine, data, CREATE | JOIN)` (`wrappers.c`). Each call site specifies the desired operation through a `t_op` enum, keeping the code concise and readable while preserving the original POSIX return codes, allowing errors to be detected and handled by the caller.
Shared values are accessed through small helper functions (`get_bool`, `set_bool`, `get_llong` and `set_llong` in `get_set.c`), which acquire the relevant mutex, perform the read or write, and release the mutex internally. This centralises synchronisation and prevents callers from accidentally leaving a mutex locked, including through early returns.
For critical sections involving multiple operations, such as `acquire_dongle()`, explicit `handle_mutex(..., LOCK/UNLOCK)` calls are used instead. Every exit path releases the mutex before returning.
- No custom event implementation is used.


## Resources

### POSIX threads
- https://medium.com/@techdhaba.training/posix-threads-pthreads-the-simplest-way-to-understand-real-multithreading-in-c-c2f591ab7a03
- https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&pp=0gcJCR0Ga94AFGB0
- PTHREADS(7) Linux Programmer's Manual - POSIX threads
- USLEEP(3) Linux Programmer's Manual
- GETTIMEOFDAY(2) Linux Programmer's Manual 

### Dining philosophers problem
- https://medium.com/@jalal92/the-dining-philosophers-7157cc05315
- https://www.youtube.com/watch?v=zOpzGHwJ3MU

### Heap in C
- https://nkugwamarkwilliam.medium.com/heap-in-c-programming-made-easy-e84a8b423660

### Use of AI
AI tools were used as development support to clarify aspects of the POSIX threads API, investigate implementation issues, and improve the wording and formatting of this README. 
The project’s architecture, algorithms, and implementation were developed and reviewed by the author. AI-generated suggestions were treated as assistance and were tested and adapted where appropriate.
