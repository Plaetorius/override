# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)

# Exploit
The home of the level00 contians a binary, `level00`, that has an SUID bit set, and his owned by level01.
I downloaded the binary on the host machine using the `scp` tool. Using Dogbolt, I decompiled it.
The `scanf()` function takes `%d`, which means it's going to convert the input into an int.
We see in the code that if the `input_value` is 5276, we get access to a terminal as level01.
Thus, we execute the binary and pass it 5276, and cat the password.
```
level00@OverRide:~$ ./level00 
***********************************
*            -Level00 -           *
***********************************
Password:5276

Authenticated!
$ cat ../level01/.pass
```
