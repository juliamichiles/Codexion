## Parser:
- [ ] test thoroughly
- [ ] accept trailing  wtspcs for fifo and edf 
- [ ] add safeguard for any arg being NULL (I think empty strings are ok)

## General:
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
