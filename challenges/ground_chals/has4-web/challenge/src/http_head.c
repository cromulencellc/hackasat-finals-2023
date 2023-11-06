
#define _POSIX_C_SOURCE 200809
#define _XOPEN_SOURCE

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <libgen.h>
#include <ctype.h>
#include <time.h>
#include <malloc.h>
#include "http.h"
#include "dictionary.h"
#include "url_decode.h"
#include "base64.h"
#include "stats.h"
#include "bst.h"
#include "debug.h"
#include "socket_io.h"

extern dictionaryType *mimeDict;
extern bstType *hoststats;
extern bstType *ua_stats;
extern int verboseLogging;

int checkAuth(int s, char *filepath, dictionaryType *headers);
int checkFile( char *fileName );


int headFile(connection_info_type *connection, serverConfigType *config, char *uri, dictionaryType *headers) {

char response[400];
char contentType[100];
char *resourcePath;
char *decodedResourcePath;
int retcode;
int s;

        if ( config == 0 )
                return -1;

        if ( headers == 0 )
                return -1;

        if ( uri == 0 )
                return -1;

        s = connection->s;

        if ( strstr(uri, ";") != 0 ) {

                sendError(s, 404);
                return 404;

        }

        if ( strstr(uri, "&&") != 0 ) {

                sendError(s, 404);
                return 404;

        }

        if ( strstr(uri, "||") != 0 ) {

                sendError(s, 404);
                return 404;

        }


        if ( uri[strlen(uri)-1] == '/') {

                resourcePath = malloc(strlen(config->docRoot) + strlen(uri) + strlen(config->defaultFile) + 2);

        }
        else {

                resourcePath = malloc(strlen(config->docRoot) + strlen(uri) + 2);
        }

        if (!resourcePath) {

                return -1;
        }

        strcpy(resourcePath, config->docRoot);
        strcat(resourcePath, uri);

        if ( resourcePath[strlen(resourcePath)-1] == '/') {

                strcat(resourcePath, config->defaultFile);
        }

        // debug("resource: %s|\n", resourcePath);

        decodedResourcePath = url_decode(resourcePath);

        // debug("decoded resource: %s|\n", decodedResourcePath);

        free(resourcePath);

        int fileCheck = checkFile( decodedResourcePath );

        if ( fileCheck != 0 ) {

                switch (fileCheck) {

                        case -1:

                                debug( "Errno: %d\n", errno);
                                perror(0);
                                sendError(s, 500);
                                retcode = 500;
                                break;

                        case 1:

                                sendError(s, 404);
                                retcode = 404;
                                break;

                        case 2:

                                sendError(s, 403);
                                retcode = 403;
                                break;

                }

                free(decodedResourcePath);

                return retcode;
        }

        retcode = checkAuth(s, decodedResourcePath, headers);

        if ( retcode != 0 ) {

                debug("Not authorized to read that file\n");
                free(decodedResourcePath);
                return retcode;
        }

        int fd = open( decodedResourcePath, O_RDONLY);

        if ( fd == -1 ) {

                // debug( "File was not found or could't be read\n");

                debug( "Errno: %d\n", errno);
                perror(0);
                sendError(s, 404);
                free(decodedResourcePath);
                return 404;
        }

        struct stat sb;

        if ( fstat( fd, &sb) != 0 ) {

                // debug( "File was not found or could't be read\n");
                sendError(s, 404);
                free(decodedResourcePath);
                close(fd);
                return 404;
        }

        char *extension;
        extension = strrchr(decodedResourcePath, '.');

        if (!extension) {

                strcpy(contentType, "application/octet-stream");
        }
        else {

                extension++;

                // debug("extension = %s\n", extension);

                char *value = findDict(mimeDict, extension);

                // debug("Found mime type = %s\n", value);

                if (value) {
                        strcpy(contentType, value);

                }
                else  {
                        strcpy(contentType, "application/octet-stream");
                }

        }

        free(decodedResourcePath);

        struct tm fileTime;
        struct tm ifModTime;
        struct tm expireDate;
        struct tm serverTime;

        char fileDateString[31];
        char serverDateString[31];
        char expireString[31];

        memset(&fileTime, 0, sizeof(struct tm));
        memset(&ifModTime, 0, sizeof(struct tm));
        memset(&serverTime, 0, sizeof(struct tm));

        gmtime_r(&sb.st_mtime, &fileTime);

        strftime(fileDateString, 30, "%a, %d %b %Y %H:%M:%S GMT", &fileTime);
        // debug( "File Modified Date = %s |\n", fileDateString);


        sb.st_mtime += 120;
        gmtime_r(&sb.st_mtime, &expireDate);
        strftime(expireString, 30, "%a, %d %b %Y %H:%M:%S GMT", &expireDate);

        // debug( "File Expire Date = %s |\n", expireString);

        time_t server_time_t = time(NULL);

        if ( server_time_t != -1 ) {

                gmtime_r(&server_time_t, &serverTime);

                strftime(serverDateString, 30, "%a, %d %b %Y %H:%M:%S GMT", &serverTime);
                // debug( "Server Date = %s |\n", serverDateString);

                sprintf(response, "HTTP/1.1 200 OK\r\n"
                                                        "Date: %s\r\n"
                                                        "Content-Length: %ld\r\n"
                                                        "Content-Type: %s\r\n"
                                                        "Last-Modified: %s\r\n"
                                                        "Expires: %s\r\n"
                                                        "Connection: close\r\n\r\n",
                                                        serverDateString, sb.st_size, contentType, fileDateString, expireString);

        }
        else {

                sprintf(response, "HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: %ld\r\n"
                                                        "Content-Type: %s\r\n"
                                                        "Last-Modified: %s\r\n"
                                                        "Expires: %s\r\n"
                                                        "Connection: close\r\n\r\n",
                                                        sb.st_size, contentType, fileDateString, expireString);
        }

        // fprintf(stderr, "Sending 200 response\n");

        send_all(s, response, strlen(response));

    close(fd);
    return 200;
}


