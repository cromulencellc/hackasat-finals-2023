#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "dictionary.h"
#include "http.h"
#include "http_errors.h"
#include "config.h"
#include "bst.h"
#include "debug.h"
#include <syslog.h>
#include "socket_io.h"
#include <poll.h>
#include <sys/sysinfo.h>

// 20 bytes to account for Method, LWS, and HTTP version
#define BUFFER_SIZE (MAX_URI + MAX_QUERY + 200)  

dictionaryType *loadMIMEtypes(char *filename);
dictionaryType *mimeDict;

void *handle_client(void *connection_info) {

connection_info_type *connection;
int flag = 1;
int retcode;
struct timespec current_time;
struct timespec start_time;
struct pollfd fds[1];
unsigned int wait_miliseconds = 250;
int total_wait_so_far;
char buffer[1024];

	connection = (connection_info_type *)connection_info;

	setsockopt(connection->s, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

	retcode = handleRequest(connection, connection->s, connection->s);

	if (retcode != 0) {

		debug("Some sort of bad request was received\n");
	}

	shutdown(connection->s, SHUT_WR);

	clock_gettime(CLOCK_MONOTONIC, &start_time);


	// wait for a while to allow the socket to be drained before closing it
	total_wait_so_far = 0;
	while( total_wait_so_far < wait_miliseconds) {

		fds[0].fd = connection->s;
		fds[0].events = POLLIN;

		retcode = poll(fds, 1, 10);

		if (retcode == -1)
			break;

		if (retcode == 1 ) {

			if (fds[0].revents & POLLIN) {

				// debug("data still left to read before closing\n");
				retcode = read(connection->s, buffer, 1024);

				if (retcode <= 0) {

					debug("Client closed the socket or an error occurred\n");
					break;
				}
				else {

					debug("Read %d byts from connection\n", retcode);
				}

			}

		}

		clock_gettime(CLOCK_MONOTONIC, &current_time);

		total_wait_so_far = (current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000) - (start_time.tv_sec * 1000 + start_time.tv_nsec / 1000000);

		// time_left_to_wait = wait_miliseconds - total_wait_so_far;
		// debug("time left to wait %d\n", time_left_to_wait);

	}

	// ok now close it
	close(connection->s);

	return((void *)0);
}

int start_server(serverConfigType *serverInfo) {

int s;
int pid;
int ls;
struct sockaddr_in peeraddr;
unsigned int addrlen = sizeof(peeraddr);
connection_info_type this_connection;

	init_404Error("www/html/404.html");

	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == -1) {

		syslog(LOG_USER, "Unable to create socket for server process\n");
		return -1;
	}

    int optval = 1;

    setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	if ( bind (s, serverInfo->listenAddr, serverInfo->addr_len) == -1) {

		syslog(LOG_USER, "Unable to bind to socket %d\n", serverInfo->port);
		return -1;
	}

	if ( listen(s, 100) == -1 ) {

		syslog(LOG_USER, "Unable to listen for connections\n");
		return -1;
	}

	if (getuid() == 0) {

		debug("Running as root--dropping privileges\n");

		/* process is running as root, drop privileges */
		if (setgid(1000) != 0)
			syslog(LOG_USER,"setgid: Unable to drop group privileges: %s", strerror(errno));
		if (setuid(1000) != 0)
			syslog(LOG_USER,"setuid: Unable to drop user privileges: %s", strerror(errno));

	}

	struct timespec request;
	struct timespec remaining;
	double difference;
	double min_request_interval;
	struct timespec current_time;
	struct timespec previous_request_time;

	previous_request_time.tv_nsec = 0;
	previous_request_time.tv_sec = 0;

	if ( serverInfo->max_requests_per_second ) {

		min_request_interval = 1.0 / serverInfo->max_requests_per_second;
	}
	else {

		min_request_interval = 0.0;
	}
	
	while(1) {

		ls = accept (s, (struct sockaddr *)&peeraddr, &addrlen);

		if ( ls < 0 ) {

			debug("%s", "Some sort of socket error occurred\n");
			return -1;

		}

		pid = fork();

		if ( pid == -1 ) {
    		syslog(LOG_USER, "[-] fork error: %s\n", strerror(errno));
    		close(s);
    		return -1;
    	}

    	if ( pid ) {
    		// debug("Client %s connected.\n", inet_ntoa(ls.sin_addr));
    		close(ls);
    	} 
		else {

			close(s);
		
			this_connection.s = ls;
			this_connection.serverInfo=serverInfo;
			memcpy((void*)&this_connection.peeraddr, (void *)&peeraddr, addrlen);

			handle_client(&this_connection);

			return 0;
		
		}

		if ( serverInfo->max_requests_per_second ) {

			clock_gettime(CLOCK_MONOTONIC, &current_time);

			difference = ( (double)current_time.tv_sec + (double)(current_time.tv_nsec)/1000000000) - ((double)previous_request_time.tv_sec + (double)(previous_request_time.tv_nsec)/1000000000);

			// debug("the difference is %f\n", difference);
			// debug("ths min request interval is %f\n", min_request_interval);
			if ( difference < min_request_interval ) {

					// debug( "Request too soon, delaying for a bit\n");
					double delay = min_request_interval - difference;

					request.tv_nsec = 1000000000 * delay;
					request.tv_sec = 0;

					while ( nanosleep(&request, &remaining) != 0 ) {

						request.tv_sec = remaining.tv_sec;
						request.tv_nsec = remaining.tv_nsec;
					}

			}

			clock_gettime(CLOCK_MONOTONIC, &previous_request_time);

		}


	} //while

}

int main(int argc, char **argv) 
{
serverConfigType serverInfo;
dictionaryType *configDict;


	debug("%s", "Webserver starting up!\n");

	syslog(LOG_USER, "Webserver starting up!\n");

    // Reap the children
    signal(SIGCHLD, SIG_IGN);

	configDict = loadINI("./server.ini");
	// printDict(configDict);

	mimeDict = loadMIMEtypes("/etc/mime.types");
	// printDict(mimeDict);

	if (loadConfig(&serverInfo, configDict) != 0 )  {
		
		debug( "Problem loading configuration data\n");
		return -1;
	}

	start_server(&serverInfo);

} //main

