# Christmas in August
**Devices Involved**: GPS
**Concepts**: GPS, Reverse Engineering, Script injection


Vulnerabilities:
- Excessively long max length on udp `reads` calls in `script_udp` app.
- Script injection for gps commands in `script_udp`app.
- Minimal error checking of responses from GPS receiver in `gps` app.
- Use of "raw" novatel commands in CFS messages rather than a stripped down version.

Combined these vulnerabilities allow the user to arbitrarily command the novatel gps receiver.

From the ICD players can learn that there are several ways to make the gps receiver output an arbitray position.

1. Modifying the rotation matrix used with `USERDATUM` command  will allow an arbitrary rotation matrix to be applied to the current position
1. The `FIX` command will allow users to specify an arbitrary position.


Either of these methods can successfully be used to bring latitude greater than 80 degrees.