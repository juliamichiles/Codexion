

## Next:
- [ ] structures.h still has Norminette errors
- [X] REVIEW README + ADD AI USAGE
- [X] a LOT of Tests
- [ ] ensure there aren't any memory leaks
- [ ] Norminette (with that flag for .h files?)
- [ ] Remove unnecessary comments (ensure remaining ones respect the norm)
- [X] adress TODOs and FIXMEs
- [ ] README - ADD A FLOWCHART!!

## At school:
- [ ] add HEADERS
- [ ] ensure its still working normally
- [ ] update to_begitignore
- [ ] run 'mv to_begitignore .gitignore' BEFORE PUSHING to intra's repo

## Tests:
- [ ] Memory leaks + threads
- [ ] Edge cases (one coder)

## Optional:
- [X] write a helper for main
- [X] Restruture directories
- [ ] modify time to use the clock function instead
- [ ] draw threads/workflow

## Restructure:
- [ ] restructure (see claude)
- [ ] .h files and make files are NOT up to date
- [ ] Untested as is
- [ ] Remove scheduler_mutex from all files
- [ ] Update makefile with new files
- [ ] I don't love the current file names and distribution...
- [ ] maybe split source in subdirectories, if so update makefile accordingly

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

## Fixed/Done:
- [X] fix init.c and simulation.c apparently
- [X] actually try to run this 
- [X] Haven't created coder threads yet
- [X] implement monitor thread
- [X] implement coder routine
- [X] fix init_dongle (each dongle should have its own heap, not a shared one)
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
