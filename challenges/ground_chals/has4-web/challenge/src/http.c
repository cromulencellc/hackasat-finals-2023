
#define _XOPEN_SOURCE

#include "http.h"
#include "dictionary.h"
#include "base64.h"
#include "url_decode.h"
#include "http_errors.h"
#include "socket_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include "crc.h"
#include "bst.h"
#include "debug.h"
#include <syslog.h>

extern dictionaryType *mimeDict;

int handleRequest(connection_info_type *connection, int in, int out) {

char method[10];
char query[MAX_QUERY];
char uri[MAX_URI];
char version[10];
dictionaryType *headers = 0;

serverConfigType *config;
struct sockaddr_in *peeraddr;

char buffer[HTTP_BUFFER_SIZE];
int ret;
int readIndex = 0;
int writeIndex;
enum state readState ;
int methodToken = 0;
int retcode;

	config=connection->serverInfo;
	peeraddr = &connection->peeraddr;
	readState = METHOD;
	readIndex = 0;
	writeIndex = 0;

	while(1) {

readMore:

		// debug("at readmore\n");
		ret = readLine(in, buffer, HTTP_BUFFER_SIZE-1);

		if (ret == -1) {

			// debug( "Line too long!\n");
			// debug( "%s\n", buffer);
	
			sendError(out, 414);
			return -1;
		}

		if (ret == 0 ) {

			debug( "out of data from socket\n");
			return -2;
		}

		// debug( "%d chars ready in this line\n", ret);
		// debug( "Line: %s\n", buffer);

		// syslog(LOG_INFO, "Request: %s", buffer);

		readIndex = 0;

		switch (readState) {

			case METHOD:

				// debug("At Method\n");
				while( buffer[readIndex] >= 'A' && buffer[readIndex] <= 'Z') {

					method[writeIndex] = buffer[readIndex];

					++writeIndex;
					++readIndex;

					if ( readIndex == ret ) 
						goto readMore;

					if ( writeIndex >= 6 ) {
					
						// debug( "Bad method length\n");
						sendError(out, 405);
						return -1;
					}

				}

				method[writeIndex] = 0;

				
				// debug( "Method = %s\n", method);

				if (strcmp(method, "GET") == 0 ) {

					methodToken = METHOD_GET;
				}
				else if (strcmp(method, "PUT") == 0) {

					methodToken = METHOD_PUT;
				}
				else if ( strcmp(method, "POST") == 0 ) {

					methodToken = METHOD_POST;
				}
				else if ( strcmp(method, "HEAD") == 0 ) {

					methodToken = METHOD_HEAD;
				}
				else {
					
					syslog(LOG_USER, "Bad Method %s received\n", method);

					// sendError(out, 405);
					// return -1;
				}

				writeIndex = 0;
				readState = URI;

			case URI:

				// debug("At URI\n");

				while( isblank( buffer[readIndex] ) ) {

					++readIndex;

					if (readIndex == ret)
						goto readMore;
				}

				while( isgraph(buffer[readIndex]) && buffer[readIndex] != '?') {

					uri[writeIndex] = buffer[readIndex];

					++writeIndex;
					++readIndex;

					if (readIndex == ret) 
						goto readMore;

					if (writeIndex >= MAX_URI-1) {

						// debug( "Bad URI length\n");
						sendError(out, 414);
						return -1;
					}
				}

				if ( buffer[readIndex] == '\r' || buffer[readIndex] == '\n') {
					
					// debug( "Bad request\n");
					sendError(out, 405);
					return -1;
				}
				uri[writeIndex] = 0;

				readState = QUERY;
				writeIndex = 0;
				// debug("URI: %s\n", uri);
				// debug("len of URI = %lu\n", strlen(uri));
			case QUERY:

				// debug("At Query\n");

				if (buffer[readIndex] != '?') {
					readState = VERSION;
					goto version;
				}
				else {

					// debug("Getting a query string\n");
				}

				++readIndex;

				if (readIndex == ret)
					goto readMore;
				
				while( isgraph(buffer[readIndex]) ) {

					// debug("Adding char: %c to query string\n", buffer[readIndex]);
					query[writeIndex] = buffer[readIndex];

					if (readIndex == ret) {
						// debug("going back to read more for query string\n");
						goto readMore;
					}
					
					++writeIndex;
					++readIndex;

					if (writeIndex == MAX_QUERY-1) {

						// debug( "Bad Query length\n");
						sendError(out,405);
						return -1;
					}

				}
				// debug("End of line for query string\n");
				query[writeIndex] = 0;

				if (strlen(query) == 0) {

					debug( "Received a zero length query string\n");
				}

				readState = VERSION;
				writeIndex = 0;

version:
			case VERSION:
				// writeIndex = 0;
				// debug("At Version\n");

				while( isspace( buffer[readIndex] ) ) {

					++readIndex;

					if (readIndex == ret)
						goto readMore;
				}

				while( isgraph(buffer[readIndex]) ) {

					if (writeIndex >= 9) {

						// debug( "Bad Version length\n");
						sendError(out, 505);
						return -1;
					}

					version[writeIndex] = buffer[readIndex];

					++writeIndex;
					++readIndex;

					if (readIndex == ret) {
						// debug("going back to read more\n");
						goto readMore;
					}
				}

				// debug("End of line for vesion\n");
				version[writeIndex] = 0;

				if (strcmp(version, "HTTP/1.1") != 0 ) {

					sendError(out, 505);
					return -1;
				}
				
				// debug( "Version: %s\n", version);

				while( buffer[readIndex] != '\n' && readIndex < ret )
					++readIndex;

				// debug( "Found the end of the request line\n");

				break;

			default:

				// debug("Hit the default case\n");
				break;

		} // switch

		break;

	} // while 

	// debug("Req: %s %s %s\n", method, uri, version);

	syslog(LOG_USER, "%s: %s %s\n", inet_ntoa(peeraddr->sin_addr), method, uri);

	int count;

	count = readLine(in, buffer, HTTP_BUFFER_SIZE);
	if ( count == -1) {

		debug( "Got error response from readLine()\n");
		return -2;
	}

	if (count == 0 ) {

		debug( "No more data to readline\n");
		return -2;
	}

	// now read the request headers line by line until a blank line is received
	while( strncmp(buffer, "\r\n", 2 ) != 0 && strncmp(buffer, "\n",1) != 0 ) {

		// to do: check for multiple headers with the same value, e.g. Host which is not allowed
		addDictbyLine(&headers, buffer, ':');

		count = readLine(in, buffer, HTTP_BUFFER_SIZE);
		if ( count == -1) {
			
			debug("Got error response from readLine2()\n");
			return -2;
		}
		else if ( count == 0 ) {
			
			debug( "No more data to read\n");
			return -2;
		}
	}

	// save the host parameter for use with virtual hosting
	char *vhost = 0;

	vhost = findDict(headers, "Host");

	// also, Host is required for HTTP/1.1
	if ( !vhost ) {

		debug("Host header field is missing!\n");
		sendError(out, 400);
		return -1;
	}

	debug("%s\n", vhost);
	
	strncpy(connection->vhostname, vhost, strlen(vhost));

	retcode = 0;
    switch (methodToken) {

        case METHOD_GET:

	        retcode = serveFile(connection, config, uri, headers);
            break;
        
        case METHOD_PUT:

			sendError(out, 501);
			retcode = 501;
            break;

		case METHOD_POST:

			retcode = handlePost(connection, config, uri, headers, version);
			break;

		case METHOD_HEAD:

			retcode = headFile(connection, config, uri, headers);
			break;

		case METHOD_DELETE:

			sendError(out, 501);
			retcode = 501;
			break;

		default:

			sendError(out, 501);
			retcode = 501;
			break;

    }

	syslog(LOG_USER, "%s: [%d] %s %s\n", inet_ntoa(peeraddr->sin_addr), retcode, method, uri);

	deleteDict(&headers);

	if ( headers != NULL) {
		
		debug( "headers should be null\n");
	}

	return retcode;
} //handleRequest

