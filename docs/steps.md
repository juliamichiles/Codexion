# Codexion: From subject to working code

Since you've already studied the concepts, I'd approach this as an implementation problem: build a small, working simulation first, then add synchronization, scheduling, and edge cases incrementally.

The subject's central challenge is that each coder needs two shared dongles to compile, must move through three phases, and must start compiling again before their burnout deadline. The program must also support both `fifo` and `edf` scheduling.

codexion-en.subject.pdf

## 1. Build it in this order

## 1

Parse and validate the arguments

Implement `main()` and validate all 8 mandatory arguments: number of coders, the four phase/deadline timings, required compile count, dongle cooldown, and scheduler (`fifo` or `edf`).

Test invalid input before writing any threading logic.

## 2

Create the core data structures

Define a `Coder` and a `Dongle`, plus a structure to hold shared simulation configuration and state.

Each coder needs an ID, its two neighboring dongles, its last compile start time, and its compile count. Keep shared state inside objects; global variables are forbidden.

## 3

Implement the lifecycle without threads first

Write and test the sequence:

`WAIT FOR DONGLES → COMPILE → DEBUG → REFACTOR → REPEAT`

Use simple sequential code to verify phase transitions, timing calculations, and compile counts. This is a temporary development step, not the final concurrent simulation.

## 4

Introduce pthreads and mutexes

Give each coder a thread. Protect shared dongle access so two coders cannot acquire the same dongle simultaneously.

A coder must acquire both dongles before compiling and release both afterward. Add synchronization around shared state and log output where needed.

## 5

Implement burnout and termination

Track when each coder last started compiling. Detect when the burnout deadline is reached, stop the simulation safely, and ensure the burnout message is printed within the subject's 10 ms requirement.

Also stop successfully when every coder reaches the required compile count.

## 6

Add cooldown and scheduling policies

Implement the dongle cooldown after release, then add `fifo` and `edf` arbitration.

Keep the acquisition mechanism separate from the coder lifecycle so you can change scheduling without rewriting the whole simulation.

## 2. What should you code first, concretely?

I'd start with these files:

```
codexion/
├── Makefile
├── README.md
└── src/
    ├── main.c
    ├── parse_args.c
    ├── simulation.c
    ├── coder.c
    ├── dongle.c
    ├── scheduler.c
    └── monitor.c
```

This is a suggested organization, not a required file layout. The subject requires C, a root Makefile, pthreads, and a custom priority queue (heap) for scheduling.

codexion-en.subject.pdf

Your first milestone should be much smaller than the complete project:

* `make` produces `codexion` with `-Wall -Wextra -Werror -pthread`.

* Invalid arguments are rejected.

* You can create the coder and dongle structures and assign the correct neighbors.

* One coder can complete a compile/debug/refactor cycle.

* Multiple coder threads can run without duplicating dongles.

Don't start with the scheduler heap. First establish that the lifecycle and resource ownership work. Then introduce the FIFO/EDF queue and test their ordering independently.

## 3. Test continuously, using tiny scenarios

Start with these cases and add more as each feature works:

| Scenario               | What to verify                                        |
| ---------------------- | ----------------------------------------------------- |
| 1 coder                | Special case: only one dongle                         |
| 2 coders               | Both compete for shared dongles                       |
| Several coders         | No duplicate dongle ownership                         |
| Short burnout deadline | Monitor detects burnout within 10 ms                  |
| Nonzero cooldown       | Released dongles aren't reused too early              |
| FIFO vs. EDF           | Requests are granted according to the selected policy |
| Compile quota reached  | Simulation stops successfully                         |

These tests target explicit requirements in the subject.

codexion-en.subject.pdf

codexion-en.subject.pdf

## 4. Your first coding session

I'd keep today's goal to just three things:

1. Create the Makefile and a minimal `main.c`.

2. Implement argument validation and the basic `Coder`, `Dongle`, and simulation structures.

3. Get a clean build and test the invalid-input cases.

Then commit that working baseline. In your next session, implement one coder's lifecycle before moving to the concurrent version.

One important design decision to make early: how will a coder request two dongles atomically from the perspective of the scheduling policy? The subject requires fair FIFO/EDF arbitration, cooldown, liveness, and a separate monitor thread. Those requirements should shape your resource-acquisition design rather than being patched in at the end.

codexion-en.subject.pdf

