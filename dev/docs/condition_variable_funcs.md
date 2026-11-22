| Function | Purpose | Important detail |
|---|---|---|
| `pthread_cond_t` | Condition variable type | Used with a mutex |
| `pthread_cond_init()` | Initialize | `attr = NULL` for defaults |
| `pthread_cond_wait()` | Wait indefinitely | Releases and reacquires mutex |
| `pthread_cond_timedwait()` | Wait with timeout | Uses absolute deadline |
| `pthread_cond_signal()` | Wake one thread | One waiter is selected |
| `pthread_cond_broadcast()` | Wake all threads | All waiters are awakened |
| `pthread_cond_destroy()` | Destroy | No threads may be waiting |
