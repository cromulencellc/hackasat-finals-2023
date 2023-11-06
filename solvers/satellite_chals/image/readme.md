# Take a picture
**Devices Involved**: Imager
**Concepts**: Spacemath, mission 

To solve this problem you must calculate the quaternion to point the camera at the target at a specific time. However, because moonlighter has a sticky z wheel you must also calculate the quaternion so that the rotation from the initial quaternion plan to the final imaging quaternion minimizes rotation about the z-axis and avoids wheel stalls. See `calc_q.py` for solution.

Note: Setting q3 to 0 does not solve this problem as rotating from a quaternion with q3=0 to a quaternion where q3 != 0 results in rotation about z.
