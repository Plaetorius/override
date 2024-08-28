[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
# Tools
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit
This exercice let's us explore the different threading mechanics.

# Explanations
We decompile the code using Dogbolt, and reverse engineer it.
There are many functions that ar enot so commonly used, so I recommend you just them using the man.
Still, I will give small brief about each of the important ones, in the order they are called:

## Functions
### `fork()`
```c
#include <unistd.h>

pid_t fork(void);
```
**Description**
Creates a new process, which starts at the point of execution where `fork()` has been called.
The newly created process (child process) is an exact copy of the calling process (the parent process) except:
- The child process has a unique process ID (PID), different from the parent's
- The child process has its copy of the parent's descriptors (see man)
- The child processes resource utilization are set to 0 (see man)
**Return values**
On success:
- Returns 0 to the child process, the PID of the child to the parent process
On failure:
- No child process is created, returns -1 and sets errno

### `wait()`
```c
#include <sys/wait.h>

pid_t wait(int *stat_loc);
```
**Description**
Susprends execution of its calling process until `stat_loc` information is available for a terminated chil process, or a signal is received,
On return from a successful `wait()` call, the `stat_loc` area contains termination information about the process that exited (see man)

Using different macros, we can get the information stored inside of `stat_loc` (see man)

### `ptrace()`
```c
#include <sys/types.h>
#include <sys/ptrace.h>

int ptrace(int request, pid_t pid, caddr_t addr, int data);
```
**Description**
Process tracing and debugging. Allows one process (the tracing process) to control another process (the traced process). Often, the traced process runs normally, but when it receives a signal, it stops. The tracing process is expected to notive this via `wait()` or the delivery of a SIGCHLD signal, examine the state of the stopped process, and cause it to terminate or continue as appropriate. All of that done through `ptrace()`.
**Arguments**
- `int request`: what operation is being performed (rest of the arguments will depend on it, except one case) (see Linux man)
	- `PTRACE_PEEKUSER`: read a word of data at iffset `addr` in the traced's USER area, that contains the register and other information about the process (see `<sys/user.h>`). The word is returned as the result of the `ptrace()` call. `data` is ignored.
	- `PTRACE_TRACEME`: Indicates that this process is being tracked by its parent. A process shouldn't do that if its parent isn't expecting to trace it (`pid`, `addr` and `data` are ignored).
	Only used by the traced process. The remaining operations are performed only by the tracing process. `pid` specifies the PID of the traced process. For operations other than `PTRACE_ATTACH`, `PTRACE_SIZE`, `PTRACE_INTERRUPT`, and `PTRACE_KILL`, the traced process must be stopped.
- `pid_t pid`: the PID of the traced process

### `kill()`
```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
```
**Description**
Send a signal to a process. Returns 0 on success. `pid` is to PID of the process to send the signal to, and `sig` the signal to send.

### `prctl()`
```c
#include <linux/prctl.h>
#include <sys/prctl.h>

int prctl(int op, ...);
```
**Description**
Used to manipulate various aspects of a thread or process. The first argument is the action, and the remainder or the arguments depends on that very first one.

At first, I thought that I would need to exploit some forking mechanism.
In fact, it's more about creating a shellcode that doesn't use `execve()`, as it's catched in the process and makes the process kill itself.

So, what do we traditionally do when we get a shellcode? We `cat /homes/levelxx/.pass`
What does cat do? It `open()`, `read()` and `write()`. No `execve()`.
So, we want to craft a payload that opens `/homes/level05/.pass`, reads its content and writes it to us on the standard output.


