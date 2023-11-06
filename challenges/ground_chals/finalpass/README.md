# finalpass
Author: meenmachine

The last password manager you'll ever need

## Challenge Description

This binary is a password manager with a race condition, no RCE was intended. The goal of the challenge is to login as admin without the credentials and without bruteforce. By reverse engineering the upload database functionality, it is possible to send a database to create new users, which the manager then automatically logs in as. This upload is done on a different thread with improper locking. By spawning the thread and uploading a file while also communicating with the original thread, it was possible to both attempt to login as well as upload a new user database at the same time. This is due to `this->current_username` being used for logging in, both in the login thread and in the upload database thread.

The race condition is not tight, and it could be lengthened by increasing the number of users on the password manager to a larger amount, since there is a check in database thread to determine whether the uploaded database username matches any of the usernames onto the system. Although the race can be easily done with only the admin user on the system with correct timing and potnetially multiple trials if necessary.

This bug was very loosly inspired by `NonHeavyFTP` in realworld 2023.

## Setup for Finals

The [admin.yaml](static/admin.yaml) can be modified to change the passwords stored in the admin database. This should be modified before creating the challenge.

To build the challenge:

`make build`
