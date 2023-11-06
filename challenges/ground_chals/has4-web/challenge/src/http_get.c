#define _POSIX_C_SOURCE 200809
#define _XOPEN_SOURCE

#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <crypt.h>
#include <dirent.h>
#include "debug.h"
#include "http.h"
#include "dictionary.h"
#include "url_decode.h"
#include "base64.h"
#include "socket_io.h"

#define NAME_MAX 256

extern dictionaryType *mimeDict;

int sendDirList(int s, char *path) {

char response[400];
struct stat sb;
int dirfd;
int result;
   
        char *dirPath = dirname(path);

        debug("dirpath %s\n", dirPath);

        result=stat(dirPath, &sb);

        printf("%x\n",sb.st_mode& S_IFMT);
        if (result == -1) {

                debug("did not get a good result from stat\n");
                sendError(s, 404);
                return 404;
        }    

        if ((sb.st_mode & S_IFMT)!= S_IFDIR) {

                debug("it is not a director\n");
                sendError(s, 404);
                return 404;
        }

        dirfd = open(dirPath, O_RDONLY);

        if (dirfd < 0 ) {

                debug("error opening directory\n");
                sendError(s, 404);
                return 404;
        }
        // int pathLen = strlen(dirPath);

        DIR *dir;
        struct dirent *entry;

        // Open the directory
        dir = opendir(dirPath);

        if (dir == NULL) {
                debug("Error opening directory\n");
                return 1;
        }

        sprintf(response, "HTTP/1.1 200 OK\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "Content-Type: text/plain\r\n"
                                "Connection: close\r\n\r\n"
        );

        send_all(s, response, strlen(response));

        // Read each entry in the directory and list the filenames
        while ((entry = readdir(dir)) != NULL) {
                // Skip "." and ".." entries as they represent the current and parent directories
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {


                        result=fstatat(dirfd, entry->d_name, &sb, 0);

                        if (result != 0) {

                                debug("problem calling fstatat()\n");
                        }

                        snprintf(response, sizeof(response), "%-64s   %ld\r\n", entry->d_name,sb.st_size);

                        char chunksize[10];

                        snprintf(chunksize, sizeof(chunksize), "%zx\r\n", strlen(response));

                        send_all(s, chunksize, strlen(chunksize));

                        send_all(s, response, strlen(response));

                }
        }

        send_all(s, "0\r\n", 3);
   
        close(dirfd);
        closedir(dir);
    return 200;
}

int checkAuth(int s, char *filepath, dictionaryType *headers) {

FILE *fp;
char accessFile[MAX_PATH_LEN];
char *authCode;
char *dir;
char *decodedCreds;
char realm[200];

        // debug("filepath = %s\n", filepath);

        if ( filepath == 0 ) {

                sendError( s, 400 );
                return 400;

        }

        // the length of the incoming file + 8 chars for ".access" is more than enough
        if (strlen(filepath) + 8 > MAX_PATH_LEN-1) {

                // debug("filepath is too long!\n");

                sendError( s, 400 );
                return 400;
        }

        dir = malloc(strlen(filepath)+1);

        if ( dir == 0 ) {

                return 500;
        }

        strcpy(dir, filepath);

        sprintf(accessFile, "%s/.access", dirname(dir));

        // debug("accessFile = %s\n", accessFile);

        fp = fopen(accessFile, "r");

        free(dir);

        // if there's no access control file, assume full access
        if (fp == NULL ) {

                // debug( "No .access file found\n");
                return 0;
        }

        // read the Realm from the first line of the file to get it out of the way (plus we might need it below)
        if ( fgets(realm, sizeof(realm), fp) == 0 ) {

                debug( "Error reading realm from .access file\n");
                sendError (s, 500);
                return 500;
        }

        if (realm[strlen(realm)-1] == '\n') {

                // debug("shortening realm string\n");

                realm[strlen(realm)-1] = 0;

        }

        if ( headers == 0 ) {
                fclose(fp);
                send401Error( s, realm );

                return 401;

        }

        // debug( "Realm = %s|\n", realm);

        authCode = findDict(headers, "Authorization");

        // if the request didn't include an Auth header, send a 401 to get one
        if ( authCode == 0 ) {

                        // debug( "There was no Auth in the request\n");
                        fclose(fp);

                        if (strlen(realm)) {

                                send401Error( s, realm );
                                return 401;
                        }
                        else {

                                send401Error(s, "");
                                return 401;
                        }
        }

        // otherwise, we got creds so let's check them
        decodedCreds = decode_base64(authCode+6);

        // debug( "Decoded creds = %s\n", decodedCreds);

        char *separator;

        separator = strrchr(decodedCreds, ':');

        if (separator == 0) {

                free(decodedCreds);
                sendError( s, 400 );

                fclose(fp);
                return 400;
        }

        // insert a NULL where the colon is so we have a nice C string for the username
        *separator = 0;

        char *username = decodedCreds;
        char *password = ++separator;

        // debug( "username: %s\npassword: %s\n", username, password);

        char *encryptedPassword;

        char line[200];
        while(1) {

                if ( fgets(line, sizeof(line), fp) == 0)
                        break;

                if (feof(fp)) {

                        break;
                }

                if ( line[0] == '#')
                        continue;

                separator = strchr(line, ':');

                if ( separator == 0 ) {

                        // debug( "Badly formatted line in .access file\n");
                        continue;
                }

                *separator = 0;

                encryptedPassword = ++separator;

                if ( strlen(encryptedPassword) < 13 ) {

                        // debug( "Badly formatted password in .access file\n");
                        continue;

                }

                encryptedPassword[13] = 0;

                if (strcmp( line, username) != 0 ) {
                        // debug( "%s name didn't match\n", line);
                        continue;
                }

                fclose(fp);

                // debug( "We must have found a match, now to check the password\n");
                // debug("encrypted password = %s\n", encryptedPassword);
                // debug( "re-crypted password = %s\n", crypt(password, encryptedPassword));

                if (strncmp( encryptedPassword, crypt(password, encryptedPassword), 13) == 0 ) {

                        // debug( "The password matched!!");
                        free(decodedCreds);
                        return 0;
                }
                else {

                        // send401Error( s, realm );
                        sendError(s, 403);
                        // debug( "the password did not match\n");
                        free(decodedCreds);
                        return 403;
                }

        }

        free(decodedCreds);
        fclose(fp);

        // debug("sending a 403 error\n");
        sendError( s, 403 );

    return 403;

}

// see if a file exists and if permissions are ok for reading it
// don't process hidden files, i.e. files that start with a period
// return -1 on error, 0 on the file exists and is readable, 1 on the file does not exist, and 2 if its is not readable
int checkFile( char *fileName ) {

    char *tmpname;
    struct stat s;

    if (!fileName) {

        return -1;
    }

    tmpname = malloc( strlen(fileName) + 1 );

    if (!tmpname) {

        return -1;
    }

    strcpy( tmpname, fileName);

    char *baseFile = basename(tmpname);

    if (baseFile[0] == '.') {

        // debug( "File starts with dot, which is no bueno\n");
        free(tmpname);
        return 1;
    }

    free(tmpname);

        if ( stat(fileName, &s) != 0)
        {

                return 1;
        }

        if (! (s.st_mode & S_IFREG)) {

                return 1;
        }
        // if ( access( fileName, F_OK) ) {

        //         debug( "Requested file does not exist\n");
        //         return 1;
        // }

        if ( access( fileName, R_OK) ) {

                // debug( "Server cannot read that file\n");
                return 2;
        }

        return 0;

}

int serveFile(connection_info_type *connection, serverConfigType *config, char *uri, dictionaryType *headers ) {

char buffer[HTTP_BUFFER_SIZE];
char response[400];
char contentType[100];
const char format1[] = "%a, %d %b %Y %H:%M:%S";
const char format2[] = "%A %d-%b-%y %H:%M:%S";
const char format3[] = "%a %b %d %H:%M:%S %Y";

char *resourcePath;
int resourcePathLen;
char *decodedResourcePath;
char *extension;
char *acceptLanguageHeader;
int s;

        debug("uri: %s\n", uri);

        s = connection->s;

        acceptLanguageHeader = findDict( headers, "Accept-Language" );

        if ( acceptLanguageHeader ) {

                if ( strstr( acceptLanguageHeader, "en;") == 0 ) {

                        sendError(s, 406 );
                        return 406;
                }
        }

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

        if ( strstr(uri, "&") != 0 ) {

                sendError(s, 404);
                return 404;

        }

        resourcePathLen = strlen(config->docRoot) + strlen(uri) + 1;

        if ( config->virtualHosts) {

                resourcePathLen += strlen(connection->vhostname) + 1;
        }

        if ( uri[strlen(uri)-1] == '/') {

                resourcePathLen += strlen(config->defaultFile);

        }
 
        resourcePath = malloc(resourcePathLen);

        if (!resourcePath) {

                return -1;
        }

        strcpy(resourcePath, config->docRoot);

        if ( config->virtualHosts) {

                strcat(resourcePath, connection->vhostname);
                strcat(resourcePath, "/");
        }

        strcat(resourcePath, uri);

        if ( resourcePath[strlen(resourcePath)-1] == '/') {

                strcat(resourcePath, config->defaultFile);
        }

        debug( "resource: %s|\n", resourcePath);

        decodedResourcePath = url_decode(resourcePath);

        debug( "decoded resource: %s|\n", decodedResourcePath);

        free(resourcePath);

        if ( strstr(decodedResourcePath, "&") != 0 ) {

                sendError(s, 404);
                return -1;
        }

        int fileCheck = checkFile( decodedResourcePath );

        if ( fileCheck != 0 ) {

                debug("checkfile is not zero\n");

                switch (fileCheck) {


                        case -1:

                                // debug( "Errno checking file to serve: %d\n", errno);
                                perror(0);
                                sendError(s, 500);
                                free(decodedResourcePath);
                                return 500;

                                break;

                        // file does not exist
                        case 1:

                                debug("File does not exist\n");

                                if ( uri[strlen(uri)-1] != '/') {

                                        sendError(s, 404);
                                        free(decodedResourcePath);

                                        return 404;
                                }
                                else {

                                        if (config->sendDirList) {

                                                debug("sending a directory list\n");
                                                int result = sendDirList(s, decodedResourcePath);
                                                free(decodedResourcePath);

                                                return result;

                                        }
                                        else {

                                                sendError(s, 404);
                                                return 404;
                                        }

                                }

                                break;

                        case 2:

                                sendError(s, 403);
                                free(decodedResourcePath);

                                return 403;
                                break;

                }

        }

        int retcode = checkAuth(s, decodedResourcePath, headers);
        if ( retcode != 0 ) {

                 debug( "Not authorized to read that file\n");
                free(decodedResourcePath);
                return retcode;
        }

        debug( "No authorization required\n");

        int fd = open( decodedResourcePath, O_RDONLY);

        if ( fd == -1 ) {

                // debug( "File was not found or could't be read\n");
                // debug( "Errno: %d\n", errno);
                // perror(0);
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

        // debug("Sending 200 response\n");

        extension = strrchr(decodedResourcePath, '.');

        if (!extension) {

                strcpy(contentType, "application/octet-stream");
        }
        else {

                extension++;

                // debug( "extension = %s\n", extension);

                char *value = findDict(mimeDict, extension);

                debug( "Found mime type = %s\n", value);

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
        struct tm serverTime;
        char fileDateString[31];
        char serverDateString[31];
        char expireString[31];

        memset(&fileTime, 0, sizeof(struct tm));
        memset(&ifModTime, 0, sizeof(struct tm));
        memset(&serverTime, 0, sizeof(struct tm));

        time_t server_time_t = time(NULL);

        if ( server_time_t != -1 ) {

                gmtime_r(&server_time_t, &serverTime);

                strftime(serverDateString, 30, "%a, %d %b %Y %H:%M:%S GMT", &serverTime);

                // debug( "Server Date = %s |\n", serverDateString);
                // debug( "Server Date2 = %s\n", ctime(&server_time_t));

        }

        gmtime_r(&sb.st_mtime, &fileTime);

        strftime(fileDateString, 30, "%a, %d %b %Y %H:%M:%S GMT", &fileTime);

        // debug( "File Modified Date = %s |\n", fileDateString);

        char *modifiedDate = 0;

        if (config->useIfModHdr) {

                modifiedDate = findDict(headers, "If-Modified-Since");

        }

        int modDateFail = 0;

        // if we coudn't get the server's time, no point in checking the if-modified-header either
        if ( server_time_t && modifiedDate ) {

                // debug( "There's a If-Modified-Since header of value: %s\n", modifiedDate);

                if ( strptime(modifiedDate, format1, &ifModTime) == 0) {

                        if ( strptime(modifiedDate, format2, &ifModTime) == 0) {

                                if ( strptime(modifiedDate, format3, &ifModTime) == 0)
                                {
                                        modDateFail = 1;
                                }
                        }
                }

                if (!modDateFail) {

                        extern long timezone;
                        tzset();
                        // debug( "%ld <= %ld\n", mktime(&ifModTime)-timezone, server_time_t);

                        // if the request time is > the server's current time, its invalid
                        if ( mktime(&ifModTime)-timezone <= server_time_t) {

                                // debug( "%ld >= %ld\n", mktime(&ifModTime), mktime(&fileTime));
                                if ( mktime(&ifModTime) >= mktime(&fileTime)) {

                                        // debug( "File mod time is earlier or equal, so don't send it\n");
                                        sendError(s, 304);
                                        return 304;

                                }
                        }
                }

        }

        struct tm expireDate;

        // two minute expire time seems reasonable...
        sb.st_mtime += 120;
        gmtime_r(&sb.st_mtime, &expireDate);

        strftime(expireString, 30, "%a, %d %b %Y %H:%M:%S GMT", &expireDate);

        // debug( "File Expire Date = %s |\n", expireString);

        if ( strlen(serverDateString)) {

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

        // debug("Sending 200 response\n");

        send_all(s, response, strlen(response));

        int readSize;

        while( (readSize=read( fd, buffer, HTTP_BUFFER_SIZE )) ) {

                if (readSize <= 0 )
                break;

                // debug( "writing %d bytes of file\n", readSize);

                send_all(s, buffer, readSize);

        }

    close(fd);
//     debug( "Finished sending file\n");
    return 200;
}
