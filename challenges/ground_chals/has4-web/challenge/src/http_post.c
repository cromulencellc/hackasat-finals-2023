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

#include "debug.h"
#include "http.h"
#include "dictionary.h"
#include "url_decode.h"
#include "base64.h"
#include "socket_io.h"


int handle_admin_pages(connection_info_type *connection, serverConfigType *config, 
            char *uri, dictionaryType *headers, unsigned char *body, unsigned body_len );

int handlePost(connection_info_type *connection, serverConfigType *config, char *uri, dictionaryType *headers, char *version) {

        int content_length = 0;
        char *content_length_header;
        char *transfer_encoding_header;
        unsigned char *body_data;
        char *expect_header;
        int retcode;

        int out = connection->s;
        int in = connection->s;

        if ( config == 0 )
                return -1;

        if ( headers == 0 ) {

                sendError(out, 400);
                return 400;
        }

        if ( uri == 0 ) {

                sendError(out, 400);
                return 400;
        }

        debug("Post uri: %s\n", uri);

        expect_header = findDict(headers, "Expect");

        if ( expect_header ) {

                if ( strcmp( expect_header, "100-continue" ) == 0 ) {

                        // debug("Got a 100-continue header so sending that response if we can\n");
                        if ( content_length <= config->maxEntitySize ) {

                                sendError( out, 100 );
                        }
                }
        }
        else {

                if ( version && strcmp(version, "HTTP/1.1") == 0 ) {

                        // debug( "need to send a continue message to this client\n");

                        if (content_length >= config->maxEntitySize) {

                                sendError(out, 100);

                        }
                }
        }

        content_length_header = 0;
        transfer_encoding_header = 0;

        content_length_header = findDict(headers, "Content-Length");

        if ( content_length_header ) {

                content_length = atoi(content_length_header);

        }
        else {

                // debug( "No Content-Length header found\n");

                transfer_encoding_header = findDict(headers, "Transfer-Encoding");

                if ( !transfer_encoding_header ) {

                        debug( "No Transfer-Encoding header found in Post\n");
                        sendError(out, 411);
                        return 411;
                }

        }

        debug("content-length = %d\n", content_length);

        // shouldn't have both a chunked encodeing and a content length
        if ( transfer_encoding_header && strcmp( transfer_encoding_header, "chunked") == 0 && content_length_header ) {

                debug( "Content-Length should not be used with Chunked encoding\n");
                sendError(out, 400);
                return 400;

        }

        if (!content_length_header && strcmp(transfer_encoding_header, "chunked")!= 0 ) {

                debug("Content-Length or chunked transfer encoding is required for a POST\n");
                sendError(out , 400);
                return 400;
        }

        if ( content_length > config->maxEntitySize ) {

                debug( "The content is too large for this server to handle\n");
                sendError(out, 413);
                return 413;
        }

        int result;
        int received_body_size;

        body_data = 0;

        result = read_body( in, headers, &body_data, config->maxEntitySize, &received_body_size );

        if ( result != 200 ) {

                sendError(out, result);

                if ( body_data ) {

                        free(body_data);
                        body_data = 0;
                }

                return result;
        }

        content_length = received_body_size;

        debug("result = %d and body_size = %d\n", result, received_body_size);
        debug("body data = %s\n", body_data);


        if (strncmp(uri, "/!admin/", strlen("/!admin/")) == 0) {

                debug("Requesting an admin page\n");
                retcode = handle_admin_pages(connection, config, uri, headers, body_data, content_length);
        }
        else {

                sendError(connection->s, 404);
                retcode = 404;
        }

        if (body_data)
            free(body_data);

    return retcode;
}


