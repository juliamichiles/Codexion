## Simulation arguments

Stores the configuration passed to the program.

```text
+--------------------------------+
|            t_args              |
+--------------------------------+
| number_of_coders               |
| time_to_burnout                |
| time_to_compile                |
| time_to_debug                  |
| time_to_refactor               |
| number_of_compiles              |
| dongle_cooldown                |
| scheduler                      |
+--------------------------------+
```

## Operation enum

Defines the possible operations on synchronization primitives and threads.

```text
       t_op
        |
        +-- LOCK
        +-- UNLOCK
        +-- INIT
        +-- DESTROY
        +-- CREATE
        +-- JOIN
        +-- DETACH
```

## Coder states

Represents the current state of a coder.

```text
        t_coder_state
              |
              +-- WAITING_FOR_DONGLES
              |
              +-- COMPILING
              |
              +-- DEBUGGING
              |
              +-- REFACTORING
              |
              +-- CODER_DONE
```

## Dongle structure

Represents a shared resource protected by a mutex.

```text
+--------------------------------+
|           t_dongle             |
+--------------------------------+
| id                             |
| mutex                          |
| avaliable_at_ms                |
+--------------------------------+
```

* `id`: Identifies the dongle.
* `mutex`: Protects access to the dongle.
* `avaliable_at_ms`: The time when the dongle becomes available again.

## Coder structure

Represents one coder and the resources and information associated with them.

```text
+--------------------------------+
|            t_coder             |
+--------------------------------+
| id                             |
| thread                         |
| state                          |
|                                |
| left_dongle  ---------------------> t_dongle
| right_dongle ---------------------> t_dongle
|                                |
| last_compile_start             |
| compile_count                  |
|                                |
| sim ------------------------------> t_simulation
+--------------------------------+
```

Each coder holds pointers to their left and right dongles and a pointer to the simulation they belong to.

## Main simulation structure

Coordinates the entire simulation.

```text
+--------------------------------------+
|             t_simulation             |
+--------------------------------------+
| args                                 |
|   +--> t_args                        |
|                                      |
| coders ------------------------------+----+
|                                      |    |
| dongles -----------------------------+--+ |
|                                      |  | |
| monitor_thread                       |  | |
|                                      |  | |
| log_mutex                            |  | |
| state_mutex                          |  | |
| scheduler_mutex                      |  | |
| scheduler_cond                       |  | |
|                                      |  | |
| stopped                              |  | |
| finshed_coders                       |  | |
+--------------------------------------+  | |
                                          | |
                 +------------------------+ |
                 |                          |
                 v                          v
          +-------------+            +-------------+
          | t_coder     |            | t_dongle    |
          | Array       |            | Array       |
          +-------------+            +-------------+
```

**Important:** `coders` and `dongles` are pointers to arrays, not individual structures.

---

## Overall relationship between the structures:


```text
                 +------------------+
                 |   t_simulation   |
                 +------------------+
                   |       |      |
                   |       |      +----------------+
                   |       |                       |
                   v       v                       v
               t_args   t_coder[]              t_dongle[]
                          |   |                    |
                          |   +-- left_dongle ------+
                          |   +-- right_dongle -----+
                          |
                          +-- sim -----------------> t_simulation
```


The simulation owns the arrays of coders and dongles. Each coder references two dongles and the simulation itself.

