# has4-web

## Challenge Description

The has4-web server is a retread of a service used in HAS3 finals. That bug was fixed, code heavily refactored, new features added, and new bug(s) inserted. Originally intended to gate access to the flight software challenge binaries, the intent was for this challenge to take 1 to 3 hours max. After 3 hours it would be sunset and the binaries provided to teams that had not accessed them already. Once the competition Moonlighter pass schedule became obviously suboptimal, it was decided to no longer gate access to the flight software at all but continue to use the web server to host them. 

The flight software was moved to a non-protected part of the server for openly being provided and the server was converted to a regular challenge with a flag file.

## Building

Use the provided Makefile and Dockerfile to create a local copy of this challenge.

## Bug(s)

The original bug was a massive buffer overflow in processing the HTTP Host field. Originally the challenge was compiled with an executable stack and ASLR disabled to allow for easy shellcode exploitation. Once the challenge was no longer being used as a gate to other challenges, NX was again enabled and ASLR re-enabled in the container. A pointer leak was added and it was statically compiled to provide as many ROP gadgets as possible. 

The challenge binary wasn't given to the teams directly. They had to recover it in one of two ways--a directory traversal bug, or an unauthenticated admin API exposed by some javascript code on the admin page. Either of these would expose the binary and reveal hidden features to retrieve file listings and retrieve files from the server that bypassed authentication controls on folders. This would also reveal the flag.txt file one directory above the web root.


## Exploitation

The hardest path to exploitation was to use the buffer overflow to create a ROP payload. An exexmplar solution is provided in the solvers folder of this release.
