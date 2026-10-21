## Simulation:
- [ ] Maybe modify destroy simulation to take a double ptr and also handle
    freeing the simulation ptr and setting it to NULL?
- [ ] write a function to handle memory allocation (what is currently being done 
    in main.c)

## init + wrappers:
- [ ] further test it? Buuut I think it's right
- [ ] norm 

## Parser:
- [ ] test thoroughly
- [ ] accept trailing  wtspcs for fifo and edf 
- [ ] add safeguard for any arg being NULL (I think empty strings are ok)

## General:
- [ ] write an alias for long long, like llong or something
- [ ] split header file in two
- [ ] ADD HEADERS!
- [ ] Norminette
- [ ] Makefile - ADD -pthread flag!!
- [ ] Check if Makefile does any unnecessary relinking
- [ ] README.md



## Questions:
- Can I print whatever error message I want?
- Should I only accept lowercase fifo/edf or should I make it case insensitive?
- Should I allow trailing spaces in each individual argument?
- How do I avoid burnouts on FIFO? Or I just don't?
- Not really a question, but I think argc must be 9, and passing all arguments as one should not be accepted
- I think obviously args must be in the same order, otherwise, how can I even know which is which...

## Fixed:
- [X] dongles_cleanup only sets th local copy of the ptr to NULL, fix it 
    - Maybe pass a double ptr? Or accept we'll have to do that oustide the func.
    - also I liked dongle_cleanup without the S better...
- [X] review wrappers.c
- [X] init.c is not complete + has typos
- [X] update .h file (some func. signatures changed, others aren't there yet)

## Notes on:

### Concepts:
- Mutexes and condition variables
- Race conditions
- Deadlocks + hoffman conditions + starvation + liveness

### Optional:
- FIFO vs. EDF

### Actual project:
- Simulation drawings/explained
- RulesNotes on:

### Concepts:
- Mutexes and condition variables
- Race conditions
- Deadlocks + hoffman conditions + starvation + liveness

### Optional:
- FIFO vs. EDF

### Actual project:
- Simulation drawings/explained
- Rules
