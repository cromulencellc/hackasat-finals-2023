# Script Kiddies
**Devices Involved**: NONE
**Concepts**: RE

Reverse engineering the `script_udp` application will reveal a function that can be triggered that sends the "Script Kiddie Message"

Reverse engineering the base libraries will reveal the the script udp message is constructed as a CFS telmetry message.