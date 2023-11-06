# Ironbank
**Devices Involved**: None
**Concepts**: Timing side channel attacks, embedded systems


The iron bank app contains several flaws that make it vulnerable to timing side channel attacks
- unlimited retries if a challenge is answered incorrectly
- naive authentication verification with delay
- exposed port information allowing any user to connect
- debug mode allowing timing information to be gathered

In general these vulnerabilities are extremely easy to see. However solving this problem on an orbital system is far from easy. In past hackasat quals challenges we've presented timing attacks before however, players were given feedback on the timing data they collected and allowed to do their analysis locally.

In this case players rarely (if ever) get any feedback on the timing data from their attacks. This means that their attacking scripts must be smart enough to.
- Conduct the attack
- Determine if the attack is succeeding
- Redmediate or try again if the attack fails
