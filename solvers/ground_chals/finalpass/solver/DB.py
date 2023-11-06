#!/usr/bin/env python3

import struct
from hashlib import sha256
from pprint import pprint


class DB:
    PASSWORD_TEMPLATE = {"name": "", "password": ""}
    SHA256_DIGEST_LEN = 32

    def __init__(self, username, password):
        self.num_passes = 0
        self.username = username
        self.password = password
        self.user = {"username": username, "pw_hash": "", "passwords": []}

        password = password if isinstance(password, bytes) else password.encode()
        self.user["pw_hash"] = sha256(password).digest()

    def dump_db(self):
        with open(f"{self.username}.db", "wb") as f:
            f.write(self.get_userdb_buf())

    def add_fake_passes(self):
        for _ in range(10):
            self.add_pw()

    def add_pw(self, name="", password=""):
        pw = dict(DB.PASSWORD_TEMPLATE)

        cur_letter = chr(ord("A") + self.num_passes)
        pw["name"] = cur_letter * 10 if not name else name
        pw["name"] += "\x00"
        pw["password"] = "F" * 10 if not password else password
        pw["password"] += "\x00"

        self.num_passes += 1

        self.user["passwords"].append(pw)

    def get_userdb_buf(self):
        buf = bytearray()
        buf.extend(self.user["username"].encode() + b"\x00")
        buf.extend(self.user["pw_hash"])
        for pw in self.user["passwords"]:
            buf.extend(pw["name"].encode())
            buf.extend(pw["password"].encode())

        buf = struct.pack("<H", len(buf) + 2) + buf

        return buf

def main():
    db = DB("admin", "supersecureadminpassword")

if __name__ == "__main__":
    main()
