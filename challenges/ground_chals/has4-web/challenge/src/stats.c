#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "bst.h"
#include "hex_utils.h"
#include "debug.h"


void hostStatsHeader( int fd ){

char outputLine[1024];
char chunkSize[7];
int lineLength;
int write_count;
int total_sent;
int amount_to_send;

    lineLength = sprintf(outputLine, "<html><head><style>table, th, td { border: 1px solid black; }</style></head><body><table><tr><th>Host</th><th>Requests</th></tr>\r\n");

    if ( lineLength <= 255 ) {

        toHex(lineLength-2, chunkSize);
        strcat(chunkSize, "\r\n");

    }
    else {

        sprintf(chunkSize, "%x\r\n", lineLength-2);
    }

	amount_to_send = strlen(chunkSize);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, chunkSize+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

	amount_to_send = strlen(outputLine);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, outputLine+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

}

void hostStatsTable( int fd, bstType *stats ) {

char outputLine[1024];
char chunkSize[8];
int lineLength;
int write_count;
int total_sent;
int amount_to_send;

    lineLength = sprintf(outputLine, "<tr><td>%s</td><td>%d</td></tr>\r\n", inet_ntoa(*(struct in_addr *)&(stats->key)), stats->count);

    if ( lineLength <= 255 ) {

        toHex(lineLength-2, chunkSize);
        strcat(chunkSize, "\r\n");

    }
    else {

        sprintf(chunkSize, "%x\r\n", (unsigned char)(lineLength-2));
    }

	amount_to_send = strlen(chunkSize);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, chunkSize+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

	amount_to_send = strlen(outputLine);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, outputLine+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}
}


void hostStatsFooter( int fd ){

char outputLine[1024];
char chunkSize[7];
int lineLength;
int write_count;
int total_sent;
int amount_to_send;

    lineLength = sprintf(outputLine, "</table></body></html>\r\n");

    if ( lineLength <= 255 ) {

        toHex(lineLength-2, chunkSize);
        strcat(chunkSize, "\r\n");

    }
    else {

        sprintf(chunkSize, "%x\r\n", lineLength-2);
    }

	amount_to_send = strlen(chunkSize);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, chunkSize+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

	amount_to_send = strlen(outputLine);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, outputLine+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}
    
    write_count = write(fd, "0\r\n\r\n", 5);

}



void uaStatsHeader( int fd ){

char outputLine[1024];
char chunkSize[7];
int lineLength;
int write_count;
int total_sent;
int amount_to_send;

    lineLength = sprintf(outputLine, "<html><head><style>table, th, td { border: 1px solid black; }</style></head><body><table><tr><th>Useragent</th><th>Count</th></tr>\r\n");

    if ( lineLength <= 255 ) {

        toHex(lineLength-2, chunkSize);
        strcat(chunkSize, "\r\n");

    }
    else {

        sprintf(chunkSize, "%x\r\n", lineLength-2);
    }

	amount_to_send = strlen(chunkSize);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, chunkSize+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

	amount_to_send = strlen(outputLine);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, outputLine+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

}

void uaStatsTable( int fd, bstType *stats ) {

char outputLine[1024];
char chunkSize[7];
int lineLength;
int write_count;
int total_sent;
int amount_to_send;

	if ( stats == 0 )
		return;

	if ( stats->data == 0 )
		return;

    lineLength = snprintf(outputLine, 1024, "<tr><td>%s</td><td>%d</td></tr>\r\n", (char *)stats->data, stats->count);

    if ( lineLength <= 255 ) {

        toHex(lineLength-2, chunkSize);
        strcat(chunkSize, "\r\n");

    }
    else {

        sprintf(chunkSize, "%x\r\n", (unsigned char) (lineLength-2));
    }



	amount_to_send = strlen(chunkSize);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, chunkSize+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

	amount_to_send = strlen(outputLine);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, outputLine+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

}


void uaStatsFooter( int fd ){

char outputLine[1024];
char chunkSize[7];
int lineLength;
int write_count;
int total_sent;
int amount_to_send;

    lineLength = sprintf(outputLine, "</table></body></html>\r\n");

    if ( lineLength <= 255 ) {

        toHex(lineLength-2, chunkSize);
        strcat(chunkSize, "\r\n");

    }
    else {

        sprintf(chunkSize, "%x\r\n", lineLength-2);
    }

	amount_to_send = strlen(chunkSize);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, chunkSize+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}

	amount_to_send = strlen(outputLine);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, outputLine+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return;
		}

		total_sent += write_count;

	}
    
    write_count = write(fd, "0\r\n\r\n", 5);

}
