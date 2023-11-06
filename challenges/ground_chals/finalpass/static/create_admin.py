#!/usr/bin/env python3

from DB import DB
import json

def main():
    with open("admin.json", "r") as f:
        admin_json = json.load(f)
    
    admindb = DB(admin_json["username"], admin_json["account_password"])

    for key, password in admin_json["passwords"].items():
        admindb.add_pw(key, password)

    admindb.dump_db()

if __name__ == "__main__":
    main()