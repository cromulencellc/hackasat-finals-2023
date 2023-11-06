
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "http_errors.h"
#include "debug.h"


char Resp100[] = "HTTP/1.1 100 Continue\r\n";
char Body100[] = "";
char Resp304[] = "HTTP/1.1 304 Not Modified\r\n";
char Resp400[] = "HTTP/1.1 400 Bad Request\r\n";
char Body400[] = "<html><body>I did not understand your request.<br></body></html>";
char Resp401[] = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"%s\"\r\nConnection: close\r\n\r\n";
char Body401[] = "";
char Resp403[] = "HTTP/1.1 403 Forbidden\r\n";
char Body403[] = "<html><body>you do not have permissions to read that file<br></body></html>";
char Resp404[] = "HTTP/1.1 404 Not Found\r\n";
char *Body404 = NULL;
char Body404_alt[] = "<html><body>Sorry I couldn't find that resource on this server!</body></html>";
char Resp405[] = "HTTP/1.1 405 Method Not Supported\r\n";
char Body405[] = "<html><body>This server does not allow this method<br></body></html>";
char Resp406[] = "HTTP/1.1 406 Not Acceptable\r\n";
char Body406[] = "<html><body>This server only supports English language content<br></body></html>";
char Resp411[] = "HTTP/1.1 411 Length Required\r\n";
char Body411[] = "<html><body>This operation requires a Content-Length or chunked Transfer-Encoding</br></body></html>";
char Resp413[] = "HTTP/1.1 413 Request Entity Too Large\r\n";
char Body413[] = "<html><body>This server is not configured to handle entity bodies of this size</body></html>";
char Resp414[] = "HTTP/1.1 414 URI Too Long\r\n\r\n";
char Body414[] = "";
char Resp500[] = "HTTP/1.1 500 Server Error\r\n";
char Body500[] = "<html><body>Sorry there was an internal server error.  Try again later<br></body></html>";
char Resp501[] = "HTTP/1.1 501 Not Implemented\r\n";
char Body501[] = "<html><body>This method is not implemented in this server</body></html>";
char Resp505[] = "HTTP/1.1 505 HTTP Version Not Supported\r\n";
char Body505[] = "<html><body>This is an HTTP 1.1 server<br></body></html>";



int init_404Error(char *contentFile404) {

struct stat sb;
int fd;
int bytes_read = 0;
int total_read = 0;

    fd = open( contentFile404, O_RDONLY);

	if ( fd == -1 ) {

		return -1;
	}


	if ( fstat( fd, &sb) != 0 ) {

		close(fd);
		return -1;

	}

	if (sb.st_size == 0) {
		close(fd);
		return -1;
	}

	Body404 = calloc(sb.st_size + 1, 1);

	if (Body404 == 0) {
		close(fd);
		return -1;
	}

	while (total_read < sb.st_size) {

		bytes_read = read(fd, Body404+total_read, sb.st_size - total_read);

		if (bytes_read == 0)
			break;

		total_read += bytes_read;
	}

	close(fd);

	return 0;
}


int send401Error( int fd, char *realm ) {

char buffer[400];
int total_sent;
int total_to_send;

	total_sent = 0;

	total_to_send = snprintf(buffer, 400, Resp401, realm);

	while( total_sent < total_to_send ) {

    	total_sent += write(fd, buffer+total_sent, total_to_send-total_sent);

		if ( total_sent < 0 ) {

			debug( "Error writing to socket\n");
			return -1;
		}

	}

	return 0;

}


int sendError(int fd, int errorCode) {

char buffer[200];
int count;
char *response;
char *body;

	// fprintf(stderr, "Sending %d response\n", errorCode);
    switch (errorCode) {

		case 100:

				response = Resp100;
				body = Body100;
			break;

		case 304: 
				response = Resp304;
				body = NULL;
			break;

        case 400:

                response = Resp400;
                body = Body400;
            break;
            
        case 403:

                response = Resp403;
                body = Body403;
            break;

        case 404:

                response = Resp404;

				if (Body404)
                	body = Body404;
				else
					body = Body404_alt;

            break;

        case 405:

                response = Resp405;
                body = Body405;
            break;

        case 406:

                response = Resp406;
                body = Body406;
            break;

        case 411:

                response = Resp411;
                body = Body411;
            break;

		case 413:

				response = Resp413;
				body = Body413;

			break;

        case 414:

                response = Resp414;
                body = Body414;

            break;

        case 500:

                response = Resp500;
                body = Body500;

            break;

        case 501:

                response = Resp501;
                body = Body501;

            break;

        case 505:

                response = Resp505;
                body = Body505;
            break;

		default:

			debug("unexpected response!");
			response = NULL;
			body = NULL;
    } // switch


	int write_count;
	int total_sent;
	int amount_to_send;
	
	amount_to_send = strlen(response);
	total_sent = 0;

	while( total_sent < amount_to_send) {

		write_count = write(fd, response+total_sent, amount_to_send - total_sent);

		if ( write_count < 0 ) {

			debug( "Error sending on socket\n");
			return -1;
		}

		total_sent += write_count;

	}

	if (body) {

    	count = sprintf(buffer, "Content-Type: text/html\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", strlen(body));

		// amount_to_send = strlen(buffer);

		amount_to_send = count;
		total_sent = 0;

		while( total_sent < amount_to_send) {

			write_count = write(fd, buffer+total_sent, amount_to_send - total_sent);

			if ( write_count < 0 ) {

				debug( "Error sending on socket\n");
				return -1;
			}

			total_sent += write_count;

		}

		amount_to_send = strlen(body);
		total_sent = 0;

		while( total_sent < amount_to_send) {

			write_count = write(fd, body+total_sent, amount_to_send - total_sent);

			if ( write_count < 0 ) {

				debug( "Error sending on socket\n");
				return -1;
			}

			total_sent += write_count;

		}


	}
	else {

		count = sprintf(buffer, "Connection: close\r\n\r\n");

		// amount_to_send = strlen(buffer);

		amount_to_send = count;
		total_sent = 0;

		while( total_sent < amount_to_send) {

			write_count = write(fd, buffer+total_sent, amount_to_send - total_sent);

			if ( write_count < 0 ) {

				debug( "Error sending on socket\n");
				return -1;
			}

			total_sent += write_count;

		}
	}

    // debug( "%s%s%s\n", response, buffer, body);

	return 0;
}


