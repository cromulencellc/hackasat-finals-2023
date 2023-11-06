#include <sys/sysinfo.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>
#include<sys/utsname.h>

#include "http.h"
#include "cJSON.h"
#include "debug.h"
#include "socket_io.h"


extern dictionaryType *mimeDict;

void send_response(connection_info_type *connection, char *response);

int adminDirList(connection_info_type *connection, char *path);
int sendFile(connection_info_type *connection, char *path);

char *get_system_stats(void) {

    char line[256];
    FILE *fp;
    char cpuid[6];
    long numcpus;
    int i,j;
    unsigned long long *stats1;
    unsigned long long *stats2;
    unsigned long long percent_idle;
    struct sysinfo systemInfo;
    struct statfs buf;
    size_t available_disk;


    debug("parsing cpu status\n");

    numcpus = sysconf(_SC_NPROCESSORS_CONF);

    debug("the system has %ld processors\n", numcpus);

    // add one to account for the summary line
    ++numcpus;

    fp = fopen("/proc/stat", "r"); 

    if (fp == NULL) {

        debug("failed to open /proc/stat\n");
        return 0;
    }

    stats1 = malloc(sizeof(unsigned long long) * 11 * numcpus);
    stats2 = malloc(sizeof(unsigned long long) * 11 * numcpus);

    for (i=0; i < numcpus; ++i) {


        if (fgets(line, 256, fp) == NULL) {

            debug("failed to read a line\n");
            free(stats1);
            free(stats2);
            fclose(fp);
            return 0;
        }

        debug("about to sscan the line\n");

        sscanf(line, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", cpuid, 
                        stats1+i*11, stats1+i*11+1, stats1+i*11+2, stats1+i*11+3, stats1+i*11+4,
                        stats1+i*11+5, stats1+i*11+6, stats1+i*11+7, stats1+i*11+8, stats1+i*11+9);


        *(stats1+i*11+10) = 0;

        for (j=0;j<10;++j)
            *(stats1+i*11+10) += *(stats1+i*11+j);

        debug("TIme 1 Total is %llu\n", *(stats1+i*11+10));

    }

    sleep(1);
    rewind(fp);

    for (i=0; i < numcpus; ++i) {


        if (fgets(line, 256, fp) == NULL) {

            debug("failed to read a line\n");
            free(stats1);
            free(stats2);
            fclose(fp);
            return 0;
        }

        debug("about to sscan the line\n");

        sscanf(line, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", cpuid, 
                        stats2+i*11, stats2+i*11+1, stats2+i*11+2, stats2+i*11+3, stats2+i*11+4,
                        stats2+i*11+5, stats2+i*11+6, stats2+i*11+7, stats2+i*11+8, stats2+i*11+9);


        *(stats2+i*11+10) = 0;

        for (j=0;j<10;++j)
            *(stats2+i*11+10) += *(stats2+i*11+j);

        debug("Time 2 Total is %llu\n", *(stats2+i*11+10));

    }

    cJSON *system_status = cJSON_CreateObject();

    percent_idle = (*(stats2+3) - *(stats1+3)) * 100 / (*(stats2+10) - *(stats1+10));


    if (cJSON_AddNumberToObject(system_status, "Overall CPU %Idle", percent_idle) == NULL ) {
        goto end;

    }

    cJSON *core_stats = cJSON_AddArrayToObject(system_status, "Cores %Idle");

    if (core_stats == NULL ) {

        goto end;
    }

    for (i=1; i < numcpus; ++i) {

        debug("difference = %llu\n", *(stats2+i*11+10) - *(stats1+i*11+10));
        percent_idle = (*(stats2+i*11+3) - *(stats1+i*11+3)) * 100 / (*(stats2+i*11+10) - *(stats1+i*11+10));
        debug("percent_idle = %llu\n", percent_idle);
        sprintf(cpuid, "CPU%2d", i-1);

        cJSON *corestat = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(corestat, cpuid, percent_idle) == NULL) {
            goto end;
        }

        cJSON_AddItemToArray(core_stats, corestat);
    }

    sysinfo(&systemInfo);
    debug("memory total = %lu, free = %lu\n", systemInfo.totalram, systemInfo.freeram);

    cJSON_AddNumberToObject(system_status, "Total Mem", systemInfo.totalram);
    cJSON_AddNumberToObject(system_status, "Free Mem", systemInfo.freeram);

    if ( statfs( "/", &buf ) == -1 ) {

        available_disk = 0;

    }
    else {
        available_disk = buf.f_bsize * buf.f_bfree;
    }
    
	debug("available disk = %lu\n", available_disk);
	
    cJSON_AddNumberToObject(system_status, "Free Disk", available_disk);

    char *stringifyversion = cJSON_Print(system_status);

end:

    cJSON_Delete(system_status);
    fclose(fp);
    free(stats1);
    free(stats2);

    return stringifyversion;
}

char *get_server_config(serverConfigType *serverInfo) {

    struct utsname name;
    char uname_result[210];

    uname(&name);
   
    snprintf(uname_result, 210, "%s %s %s %lld\n", name.sysname, name.release, name.version, name.machine);

    cJSON *system_config = cJSON_CreateObject();


    cJSON_AddStringToObject(system_config, "Server Name", serverInfo->serverName);
    cJSON_AddStringToObject(system_config, "DocumentRoot", serverInfo->docRoot);
    cJSON_AddStringToObject(system_config, "Default File", serverInfo->defaultFile);
    cJSON_AddNumberToObject(system_config, "Listen Port", serverInfo->port);
    cJSON_AddNumberToObject(system_config, "Max Entity Size", serverInfo->maxEntitySize);
    cJSON_AddNumberToObject(system_config, "Max Request/sec", serverInfo->max_requests_per_second);
    cJSON_AddStringToObject(system_config, "Host OS", uname_result);

    if (serverInfo->virtualHosts == 1) {

        cJSON_AddStringToObject(system_config, "Virtual Hosts", "true");

    }
    else {

        cJSON_AddStringToObject(system_config, "Virtual Hosts", "false");

    }

    if (serverInfo->use_stdout == 1) {

        cJSON_AddStringToObject(system_config, "Use STDOUT", "true");

    }
    else {

        cJSON_AddStringToObject(system_config, "Use STDOUT", "false");

    }

    if (serverInfo->useIfModHdr == 1) {

        cJSON_AddStringToObject(system_config, "Use IfModSince Hdr", "true");

    }
    else {

        cJSON_AddStringToObject(system_config, "Use IfModSince Hdr", "false");

    }

    if (serverInfo->sendDirList == 1) {

        cJSON_AddStringToObject(system_config, "Send Dir Listings", "true");

    }
    else {

        cJSON_AddStringToObject(system_config, "Send Dir Listings", "false");

    }



    char *stringifyversion = cJSON_Print(system_config);

    cJSON_Delete(system_config);

    return stringifyversion;

}

int send_dashboard(connection_info_type *connection) {

// char response_data[400];

    return 0;

}

int handle_admin_pages(connection_info_type *connection, serverConfigType *config, 
            char *uri, dictionaryType *headers, unsigned char *body, unsigned body_len ) {

char *uri_base;
char *sys_stats;
cJSON *jsonRequest;
const cJSON *path = 0;
const cJSON *cmd = 0;
int retcode;

    // must have a URI and a message body for this API
    if (uri == 0 || body_len == 0) {

        sendError(connection->s, 400);
        return 400;
    }

    char *content_type = findDict( headers, "Content-Type");

    if (content_type == 0 ) {

        debug("Content-Type header is required\n");
        sendError(connection->s, 400);
        return 400;
    }

    debug("content-type = %s\n", content_type);

    if (strcmp(content_type, "application/json") != 0) {

        debug("Content-Type %s is not valid\n", content_type);
        sendError(connection->s, 400);
        return 400;

    }

    jsonRequest = cJSON_Parse((char *)body);

    if (jsonRequest == NULL) {

        debug("Unable to parse POST body as JSON\n");
        sendError(connection->s, 400);
        return 400;
    }

    uri_base = basename(uri);

    if ( strcmp(uri_base, "dashboard") == 0 ) {

        // do dashboard
        debug("dashboard requested\n");
        sys_stats = get_system_stats();

        send_response(connection, sys_stats);

        cJSON_free(sys_stats);

        return 200;
    }
    else if ( strcmp(uri_base, "config") == 0 ) {

        syslog(LOG_USER, "The Admin Config page was requested");

        debug("config requested\n");
        sys_stats = get_server_config(connection->serverInfo);
        debug("%s\n",sys_stats);
        send_response(connection, sys_stats);

        cJSON_free(sys_stats);

        return 200;
    }
    else if ( strcmp(uri_base, "files") == 0 ) {


        debug("Files requested\n");

        cmd = cJSON_GetObjectItemCaseSensitive(jsonRequest, "file");

        path = cJSON_GetObjectItemCaseSensitive(jsonRequest, "path");

        if (cmd == 0 || path == 0 ) {

            sendError(connection->s, 400);
            cJSON_Delete(jsonRequest);
            return 400;            

        }

        if (strcmp(cmd->valuestring, "list") == 0 ) {
            if (cJSON_IsString(path) && (path->valuestring != NULL))
            {
                syslog(LOG_USER, "The Admin Dir Listing service was requested");

                retcode=adminDirList(connection, path->valuestring);
                return retcode;
            }
            else {

                sendError(connection->s, 400);
                cJSON_Delete(jsonRequest);
                return 400;
            }
        }
        else if ( strcmp(cmd->valuestring, "get") == 0 ) {
            if (cJSON_IsString(path) && (path->valuestring != NULL))
            {
                syslog(LOG_USER, "The Admin Files service requested %s\n", path->valuestring);
                retcode=sendFile(connection, path->valuestring);
                return retcode;

            }
            else {

                sendError(connection->s, 400);
                cJSON_Delete(jsonRequest);
                return 400;
            }


        }
        else {
                sendError(connection->s, 400);
                cJSON_Delete(jsonRequest);
                return 400;

        }
    }
    else {

        syslog(LOG_USER, "Unexpected Admin feature %s", uri);
        sendError(connection->s, 404);
        debug("Unknown uri received: %s\n", uri);
        return 404;
    }

    return 0;
}


void send_response(connection_info_type *connection, char *response) {
    char response_header[400];
    struct tm serverTime;
    char serverDateString[31];

    memset(&serverTime, 0, sizeof(struct tm));

    time_t server_time_t = time(NULL);

    if ( server_time_t != -1 ) {

            gmtime_r(&server_time_t, &serverTime);

            strftime(serverDateString, 30, "%a, %d %b %Y %H:%M:%S GMT", &serverTime);

            // debug( "Server Date = %s |\n", serverDateString);

            sprintf(response_header, "HTTP/1.1 200 OK\r\n"
                                                    "Date: %s\r\n"
                                                    "Content-Length: %ld\r\n"
                                                    "Content-Type: %s\r\n"
                                                    "Connection: close\r\n\r\n",
                                                    serverDateString, strlen(response), "application/json");

    }
    else {

            sprintf(response_header, "HTTP/1.1 200 OK\r\n"
                                                    "Content-Length: %ld\r\n"
                                                    "Content-Type: %s\r\n"
                                                    "Connection: close\r\n\r\n",
                                                    strlen(response), "application/json");

    }

    send_all(connection->s, response_header, strlen(response_header));

    send_all(connection->s, response, strlen(response));

}

int adminDirList(connection_info_type *connection, char *path) {

struct stat sb;
int dirfd;
int result;
int retcode;

    debug("path %s\n", path);

    result=stat(path, &sb);

    if (result == -1) {

            debug("did not get a good result from stat on directory\n");
            sendError(connection->s, 404);
            return 404;
    }    

    if ((sb.st_mode & S_IFMT)!= S_IFDIR) {

            debug("it is not a director\n");
            sendError(connection->s, 404);
            return 404;
    }

    dirfd = open(path, O_RDONLY);

    if (dirfd < 0 ) {

            debug("error opening directory\n");
            sendError(connection->s, 404);
            return 404;
    }

    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path);

    if (dir == NULL) {
            perror("Error opening directory");
            return 500;
    }

    cJSON *filelist_response = cJSON_CreateObject();

    if (cJSON_AddStringToObject(filelist_response, "Status", "Success") == NULL ) {

        goto end;
    }

    cJSON *filelist = cJSON_AddArrayToObject(filelist_response, "Files");

    if (filelist == NULL ) {

        goto end;
    }

        // Read each entry in the directory and list the filenames
        while ((entry = readdir(dir)) != NULL) {
                // Skip "." and ".." entries as they represent the current and parent directories
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {


                        result=fstatat(dirfd, entry->d_name, &sb, 0);

                        if (result != 0) {

                            debug("problem calling fstatat()\n");
                        }

                        cJSON *fileentry = cJSON_CreateObject();

                        if (cJSON_AddStringToObject(fileentry, "name", entry->d_name) == NULL) {
                            retcode=500;
                            goto end;
                        }
                        if (cJSON_AddNumberToObject(fileentry, "size", sb.st_size) == NULL) {
                            retcode=500;
                            goto end;
                        }

                        debug("%s filetype = %d\n", entry->d_name, entry->d_type);

                        switch (entry->d_type) {

                            case 4:
                                cJSON_AddStringToObject(fileentry, "type", "directory");

                                break;
                            case 8:
                                cJSON_AddStringToObject(fileentry, "type", "regular");
                                break;
                            default:
                                cJSON_AddStringToObject(fileentry, "type", "other");

                        }

                        cJSON_AddItemToArray(filelist, fileentry);

                }
        }

        char *stringifyversion = cJSON_Print(filelist_response);

        if (stringifyversion == 0) {

            retcode = 500;
            goto end;
        }
        debug("%s\n", stringifyversion);

        send_response(connection, stringifyversion);

        retcode = 200;
end:
    cJSON_free(stringifyversion);
    cJSON_Delete(filelist_response);
    close(dirfd);
    closedir(dir);

    return retcode;
}

int sendFile(connection_info_type *connection, char *path) {

struct stat sb;
int fd;
int result;
char response[400];
char *extension;
char contentType[100];
char chunksize[10];
int bytes_read;
unsigned char send_buffer[1024];
char *pathcopy;
char *filename;

    debug("SendFile: %s\n", path);

    result=stat(path, &sb);

    // printf("%x\n",sb.st_mode& S_IFMT);

    if (result == -1) {

            debug("did not get a good result from stat\n");
            sendError(connection->s, 404);
            return 404;
    }    

    if (strstr(path, "flag.txt")!= 0 ) {

        sendError(connection->s, 403 );
        return 403;
    }

    if ((sb.st_mode & S_IFMT) != S_IFREG) {

            debug("it is not a regular file\n");
            sendError(connection->s, 400);
            return 400;
    }

    pathcopy = malloc( strlen(path)+1);

    strcpy(pathcopy, path);

    filename=basename(pathcopy);


    fd = open(path, O_RDONLY);

    if (fd < 0 ) {

        sendError(connection->s, 500);
        free(pathcopy);
        return 500;
    }

    extension = strrchr(filename, '.');

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

    free(pathcopy);

    // sprintf(response, "HTTP/1.1 200 OK\r\n"
    //                         "Transfer-Encoding: chunked\r\n"
    //                         "Content-Type: %s\r\n"
    //                         "Connection: close\r\n\r\n", contentType
    // );
    sprintf(response, "HTTP/1.1 200 OK\r\n"
                            // "Transfer-Encoding: chunked\r\n"
                            "Content-Length: %d\r\n"
                            "Content-Type: %s\r\n"
                            "Connection: close\r\n\r\n", sb.st_size, contentType
    );

    send_all(connection->s, response, strlen(response));

    do {

        // printf("reading more data\n");

        bytes_read = read(fd, send_buffer, 1024);

        // printf("bytes = %d\n", bytes_read);


        // snprintf(chunksize, 10, "%x\r\n", bytes_read);

        // printf("chunk size = %s\n", chunksize);
        // send_all(connection->s, chunksize, strlen(chunksize));

        if (bytes_read) {

            send_all(connection->s, (char *)send_buffer, bytes_read);
        }

    } while ( bytes_read > 0 );

    close(fd);

return 200;

}
