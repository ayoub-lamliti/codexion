*This project has been created as part of the 42 curriculum by alamliti.*

## Description
Codexion is a complex multi-threading and concurrency simulation written in C, inspired by the classic "Dining Philosophers" problem. The simulation orchestrates multiple coders operating in a shared workspace who must alternate between compiling, debugging, and refactoring without starving. To compile, a coder must acquire two shared resources (USB dongles). If a coder fails to compile within a strictly defined `time_to_burnout` limit, they die, and the simulation terminates. The challenge lies in managing shared state, avoiding deadlocks, and implementing a strict resource scheduler (FIFO or EDF) using POSIX threads and mutexes.

## Instructions
**Compilation:**
The project includes a `Makefile` that complies with the required flags (`-Wall -Wextra -Werror -pthread`).
* `make`: Compiles the executable `codexion`.
* `make clean`: Removes object files.
* `make fclean`: Removes object files and the executable.
* `make re`: Recompiles the project from scratch.

**Execution:**
Run the program with the following mandatory arguments:
`./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler`

*Example:*
`./codexion 5 800 200 200 200 5 10 fifo`

## Resources
During the development of this project, the following resources and methodologies were utilized:
* **POSIX Threads Programming:** Standard documentation on `pthread_create`, `pthread_join`, and `pthread_mutex`.
* **Concurrency Models:** Academic materials regarding Coffman's conditions for deadlock and strategies for starvation prevention.
* **AI Usage:** Artificial Intelligence was utilized strictly for code reviews, architectural critique (identifying tight coupling and race conditions), and generating edge-case testing scenarios. AI was explicitly instructed to act as a strict reviewer to pressure-test the logic rather than generate raw functional code.

## Blocking cases handled
To ensure the simulation remains robust and strictly adheres to the liveness requirements, the following concurrency issues were addressed:
* **Deadlock Prevention (Coffman's Conditions):** A strict lock hierarchy is enforced during dongle acquisition. Coders with even IDs attempt to lock their right dongle first, while odd-ID coders lock their left dongle first. This inherently prevents the "circular wait" condition.
* **Starvation Prevention & Cooldown:** The implementation must utilize the mandatory scheduling policies (FIFO/EDF) paired with the `dongle_cooldown` restriction to ensure fair arbitration among coders competing for the same hardware.
* **Log Serialization:** All terminal outputs are strictly protected by a dedicated `sys->log` mutex. This guarantees that messages (e.g., "is compiling", "burned out") never interleave or corrupt the standard output.
* **Precise Burnout Detection:** A detached monitor thread constantly iterates over the coders. State variables are locked for mere nanoseconds to read the `last_compile_start`, ensuring the burnout log is printed within the strict 10 ms tolerance window.

## Thread synchronization mechanisms
The architecture heavily relies on granular POSIX thread primitives to coordinate state safely:
* **`pthread_mutex_t` for Dongles:** Each USB dongle is protected by its own mutex to ensure mutually exclusive access.
* **`pthread_mutex_t` for State:** A central `sys->state` mutex prevents data races when the monitor thread reads `last_compile_start` while a coder thread is simultaneously updating it upon starting a compilation phase.
* **Condition Variables & Queues:** To implement the strict FIFO and Earliest Deadline First (EDF) arbitration, condition variables (`pthread_cond_t`) and priority queues are required to manage coders waiting for resources, waking them only when their specific scheduling criteria are met.