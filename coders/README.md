*This project has been created as part of the 42 curriculum by alamliti.*

---

# Codexion

> Master the race for resources before the deadline masters you.

---

## Description

Codexion is a concurrency simulation inspired by the classic Dining Philosophers problem. A group of coders sit in a circular co-working hub, each needing two USB dongles simultaneously to compile their quantum code. After compiling, they debug, then refactor, then try to compile again — indefinitely, until either every coder has compiled the required number of times, or one burns out from lack of compiling.

The challenge is to synchronize access to shared dongles across multiple POSIX threads, enforce a post-release cooldown per dongle, and implement fair arbitration (FIFO or EDF) via a priority queue — all without deadlocks, starvation, or data races.

---

## Instructions

### Compilation

```bash
make
```

Compiles with `-Wall -Wextra -Werror -pthread`. The binary is named `codexion`.

### Usage

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) |
| `time_to_burnout` | Max ms a coder can go without starting a new compile |
| `time_to_compile` | Ms spent holding both dongles while compiling |
| `time_to_debug` | Ms spent debugging after compiling |
| `time_to_refactor` | Ms spent refactoring before the next compile attempt |
| `number_of_compiles_required` | Simulation ends when every coder reaches this count |
| `dongle_cooldown` | Ms a dongle is unavailable after being released |
| `scheduler` | `fifo` (arrival order) or `edf` (earliest deadline first) |

All arguments are mandatory. Inputs of `0`, negatives, non-integers, or a scheduler other than `fifo`/`edf` are rejected.

### Examples

```bash
# 5 coders, 800ms burnout, 200ms compile, 200ms debug, 200ms refactor,
# 10 compiles required, 50ms dongle cooldown, FIFO scheduling
./codexion 5 800 200 200 200 10 50 fifo

# Same parameters with EDF scheduling
./codexion 5 800 200 200 200 10 50 edf

# Single coder — will burn out (no second dongle available)
./codexion 1 500 200 200 200 5 50 fifo
```

### Log format

```
timestamp_in_ms  coder_id  has taken a dongle
timestamp_in_ms  coder_id  is compiling
timestamp_in_ms  coder_id  is debugging
timestamp_in_ms  coder_id  is refactoring
timestamp_in_ms  coder_id  burned out
```

### Cleanup

```bash
make clean    # removes object files
make fclean   # removes object files and binary
make re       # fclean + all
```

---

## Blocking Cases Handled

### Deadlock Prevention

The classic circular-wait deadlock in dining philosophers is broken by **asymmetric dongle acquisition order**. Even-numbered coders grab their right dongle first, odd-numbered coders grab their left dongle first (`assign_dongles` in `simulation.c`). This breaks the circular dependency that would otherwise form when every coder simultaneously holds one dongle and waits for the other.

The four Coffman conditions are addressed as follows:
- **Mutual exclusion**: necessary and preserved (each dongle has one user at a time).
- **Hold and wait**: mitigated by the odd/even ordering — a global cycle cannot form.
- **No preemption**: preserved by design; dongles are never forcibly taken.
- **Circular wait**: eliminated by the asymmetric acquisition order.

### Starvation Prevention

When multiple coders queue for the same dongle, the scheduler decides who goes next:

- **FIFO**: the coder whose `wait_start_time` is earliest is served first. This is a strict arrival-order guarantee — no coder waits behind a latecomer.
- **EDF (Earliest Deadline First)**: the coder whose deadline (`last_compile_start + time_to_burnout`) is soonest is served first. This directly minimizes the risk of burnout under contention.

Both policies are backed by a **min-heap** (priority queue) stored per dongle. The heap key switches between `wait_start_time` (FIFO) and `last_compile_start` (EDF) via the `scheduler` flag. This guarantees bounded waiting times under both policies, as long as the simulation parameters are feasible.

### Dongle Cooldown

After a dongle is released via `drop_dongle`, its `last_time_cooldown` is recorded. Any coder that wins the queue must still wait out the remaining cooldown (`dongle_cooldown - elapsed`) before actually acquiring the dongle. This wait occurs **outside** the dongle's mutex to avoid holding the lock during sleep. The cooldown cannot be bypassed: `take_dongle` always checks the remaining time after being woken by the condition variable.

### Precise Burnout Detection

A dedicated **monitor thread** polls all coders every 1 ms. For each coder it locks `coder->lock`, reads `last_compile_start`, and compares it to `get_time_by_ms()`. If the elapsed time exceeds `time_to_burnout`, it sets `stop_simulation = 1`, prints the burnout message while holding the log mutex, and broadcasts on all dongle condition variables to wake any threads blocked in `pthread_cond_wait`. This ensures the burnout log appears within the required 10 ms window.

### Log Serialization

All `printf` calls that produce simulation output go through `logger()` in `utils.c`, which holds the `sys->log` mutex for the entire duration of the print. This prevents interleaved output even when multiple threads attempt to log simultaneously. The monitor also acquires `sys->log` before printing the burnout message.

---

## Thread Synchronization Mechanisms

### Mutexes used

| Mutex | Protects |
|---|---|
| `dongle->lock` | `dongle->is_available`, `dongle->last_time_cooldown`, `dongle->queue` |
| `coder->lock` | `coder->last_compile_start`, `coder->number_of_compiles` |
| `sys->state` | `sys->stop_simulation` |
| `sys->log` | All stdout output |

Every read and write of a shared field goes through its owning mutex. There are no unprotected accesses.

### Condition variable: `dongle->wait_queue`

Each dongle has one `pthread_cond_t`. When a coder finds the dongle taken or not at the front of the queue, it calls `pthread_cond_wait(&dongle->wait_queue, &dongle->lock)`. The coder re-checks both conditions on wake-up (spurious wakes are safe because of the `while` loop).

`drop_dongle` calls `pthread_cond_broadcast` — not `pthread_cond_signal` — so all waiting coders re-evaluate; the heap then selects the correct one to proceed.

The monitor calls `pthread_cond_broadcast` on all dongle condition variables when it sets `stop_simulation`, guaranteeing that no coder thread remains blocked in `pthread_cond_wait` after the simulation ends.

### Race-condition prevention example

```c
// take_dongle — coder enters the queue and waits
pthread_mutex_lock(&dongle->lock);
coder->wait_start_time = get_time_by_ms();   // recorded under lock
heap_push(dongle->queue, coder);             // insertion under lock
while (dongle->is_available == 0
       || dongle->queue->waiting_list[0] != coder)
{
    pthread_cond_wait(&dongle->wait_queue, &dongle->lock);
    if (check_simulation_stopped(coder->sys)) // checked before proceeding
    {
        coder->is_in_queue = 0;
        pthread_mutex_unlock(&dongle->lock);
        return ;
    }
}
heap_pop(dongle->queue);
dongle->is_available = 0;
pthread_mutex_unlock(&dongle->lock);
// cooldown sleep happens outside the lock
```

The `while` loop (not `if`) handles spurious wakeups and cases where a different coder was granted the dongle. The simulation-stopped check inside the loop prevents a coder from proceeding after termination has been signalled.

### Monitor-to-coder communication

The monitor reads `coder->lock`-protected fields and writes to `sys->state`-protected `stop_simulation`. Coders read `stop_simulation` via `check_simulation_stopped()`, which always locks `sys->state`. No field is ever read or written without holding its corresponding mutex, so there is no torn-read or visibility hazard.

---

## Resources

### Concurrency & POSIX threads
- [POSIX Threads Programming — Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [The Little Book of Semaphores — Allen B. Downey](https://greenteapress.com/wp/semaphores/)
- Dijkstra, E. W. — *"Hierarchical Ordering of Sequential Processes"* (1971), original dining philosophers formulation
- [Linux `pthread_cond_wait` man page](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)

### Scheduling algorithms
- Liu, C. L. & Layland, J. W. — *"Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment"*, JACM 1973 (EDF)
- [Wikipedia — Earliest Deadline First Scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### Priority queues
- Cormen et al. — *Introduction to Algorithms*, Chapter 6 (Heapsort and priority queues)

### AI usage

Claude (Anthropic) was used during this project for the following tasks:

- **Design review**: discussing the min-heap approach for FIFO/EDF arbitration and validating the odd/even dongle-ordering deadlock prevention strategy.
- **Code critique**: identifying potential race conditions, verifying that every shared field access is covered by the correct mutex, and checking for missed wakeup scenarios.
- **README generation**: drafting the initial structure and content of this README based on the source files and project specification, then refined manually.