
#ifndef config_type_h

#define config_type_h

#include <sys/types.h>
#include <sys/socket.h>

#include "dictionary.h"

#define MAX_PATH_SIZE 255
#define MAX_FILENAME_LENGTH 255
#define MAX_SERVER_NAME_LENGTH 255

typedef struct {
	
	int use_stdout;
	unsigned short int port;
	unsigned long maxEntitySize;
	char docRoot[MAX_PATH_SIZE];
	char defaultFile[MAX_FILENAME_LENGTH];
	char serverName[MAX_SERVER_NAME_LENGTH];
	struct sockaddr *listenAddr;
	int addr_len;
	int max_requests_per_second;
	unsigned char virtualHosts;
	unsigned char useIfModHdr;
	unsigned char sendDirList;

} serverConfigType;


int loadConfig( serverConfigType *serverConfigData, dictionaryType *configDict);


#endif