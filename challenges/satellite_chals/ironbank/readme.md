# Iron Bank

The IronBank is an application that will run in the background in parallel to the CFS apps. 

The purpose of this app is to maintain confidentiality of user scripts loaded to the satellite. We achieve this by keeping the scripts in a "safe" area and only moving the scripts into the "run area" when they are started.

The ironbank has some interesting commands that require authentication to execute
- Move a script into the run area
- Move a script into the download area
- Move a flag into the download area

The authentication scheme is vulnerable to timing side channel attacks allowing players to
- get a flag
- steal other players scripts

## Development Notes
The Iron Bank is not a CFS app and should use no CFS isms.

The Iron Bank only communicates with UDP over localhost.

The Iron Bank communicates with users via a UDP interface. 


