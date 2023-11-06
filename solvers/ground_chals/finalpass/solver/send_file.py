#!/usr/bin/env python3

import os
import sys
from pwn import *

context.log_level = "debug"

def main(filename, port):
    if(not os.path.exists(filename) or os.path.isdir(filename)):
        print(f"{filename} doesn't exist")
        exit(-1)
    
    p = remote("localhost", port)

    with open(filename, "rb") as f:
        contents = f.read()
    
    p.send(p32(len(contents)))
    p.send(contents)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("usage ./send_file.py <filename> <port>")
    
    main(sys.argv[1], sys.argv[2])