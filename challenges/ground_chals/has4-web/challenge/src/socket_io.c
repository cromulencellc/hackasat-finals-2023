#include <stdio.h>
#include <unistd.h>

int send_all(int s, char *buffer, int len) {

int write_count;
int total_sent;

    total_sent = 0;

    while( total_sent < len ) {

            write_count = write(s, buffer+total_sent, len - total_sent);

            if ( write_count < 0 ) {

                    // debug( "Error sending on socket\n");
                    return -1;
            }

            total_sent += write_count;

    }

    return 0;
}


int readLine( int fd, char *buffer, int maxLen ) {

int writePos;

	writePos = 0;
	int count;

	if ( buffer == 0 ) 
		return -1;

	buffer[0] = 0;

	while( writePos < maxLen ){

		count = read(fd, buffer+writePos, 1);

		if (count == 0) {

			// debug("No more data to read\n");
			return 0;
		}

		if (buffer[writePos] == '\n')
			break;

		++writePos;

	} 

	// we hit the end of the buffer w/o finding a newline, bad line read
	if ( writePos == maxLen )
		return -1;

	else{

		buffer[writePos+1] = 0;
		return writePos;
	}

}


