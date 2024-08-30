# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit 
Level06 is about thinking outside of the box.

# Explanation
We want `len` to be equal to `val` in `auth`.

Using the reverse-engineered source code, managed to craft a debug version of the code (see `Resources/debug.c`).
When you input a string in it, it gives you the computed value `val`.
Thus, you just have to pass a string starting with "%d" so `scanf()` will take an int, and pass it the value `val` you have been given on your first run (use the same string).

```
➜  Resources git:(main) ✗ gcc test.c -o test && ./test
***********************************
*               level06           *
***********************************
-> Enter Login: %daaaaaa
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 1337
Len: 1337
Val: 6235011
➜  Resources git:(main) ✗ gcc test.c -o test && ./test
***********************************
*               level06           *
***********************************
-> Enter Login: %daaaaaa
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6235011
Len: 6235011
Val: 6235011
Authenticated!
sh-3.2$ 
```

Do it again on the machine:
```
level06@OverRide:~$ ./level06
***********************************
*		level06		  *
***********************************
-> Enter Login: %daaaaaa
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6235011
Authenticated!
$ cat /home/users/level07/.pass
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
$
```