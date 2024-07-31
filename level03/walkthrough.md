# Tools
[Dogbolt - Decompiler Explorator](https://dogbolt.org/)
[Cutter - Reverse Engineering Tool](https://cutter.re/)
[GNU DeBugger - GDB](https://fr.wikipedia.org/wiki/GNU_Debugger)

# Exploit
This level relies heavily on the comprehension of the XOR mechanic. 
It mainly relies on intuition rather than practical knowledge.


## XOR
To fiddle with it, you can check out the `xor_decoder.c` file in the Resources folder.

An example is worth a thousand words:
  11000100
^ 10010110
\---------
  01010010

The rules are simple:

A   | B    | A ^ B
----|------|------
False|False|False
True|False|True
False|True|True
True|True|False

When you XOR a string with a character (applying the XOR operation to every character of the string), you can retrieve the original string from the encoded string using the character.
But, and that's what is interesting for us, we can also retrieve the character used for encoding using the original string and the encoded string. 

That's what being done in the function `xor_strings` in `xor_decoder.c`.

## Beating the encoding
If we XOR each character of the original string with its counterpart in the encoded string, we retrieve the encoding character. Here, it's `0x12` = 18.

When we look at the code of `level03`, we see that the character passed to `decrypt` is going to be the result of 322424845 - `number passed`. So, we need to pass 322424845 - 18 = 322424827.

Solved.
```
level03@OverRide:~$ ./level03
***********************************
*		level03		**
***********************************
Password:322424827
$ cat /home/users/level04/.pass
```

# Resources
[Wikipedia - XOR](https://en.wikipedia.org/wiki/Exclusive_or)