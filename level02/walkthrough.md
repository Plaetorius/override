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