#ifndef http_h
#define http_h

#include "dictionary.h"
#include "config.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#define  METHOD_GET 1
#define  METHOD_PUT 2
#define METHOD_POST 3
#define METHOD_HEAD 4
#define METHOD_DELETE 5
#define MAX_URI 2048
#define MAX_QUERY 2048

enum state { METHOD = 0, URI, QUERY, VERSION, HEADERS, BODY};

typedef struct {

	int s;
	serverConfigType *serverInfo;
	struct sockaddr_in peeraddr;
	char vhostname[254];

} connection_info_type;


int handleRequest(connection_info_type *connection, int s, int t);
int serveFile(connection_info_type *connection, serverConfigType *config, char *uri, dictionaryType *headers );
int sendError(int fd, int errorCode);
int send401Error( int fd, char *realm );
int handlePost(connection_info_type *connection, serverConfigType *config, char *uri, dictionaryType *headers, char *version);
int headFile(connection_info_type *connection, serverConfigType *config, char *uri, dictionaryType *headers );
char *handle_encoded_form_data( char *data);
char *form_decode(char *form_data);
int read_body(int in, dictionaryType *headers, unsigned char **body_data, int maxBodySize, int *received_size);

#define HTTP_BUFFER_SIZE 4096
#define MAX_PATH_LEN 256

#endif 