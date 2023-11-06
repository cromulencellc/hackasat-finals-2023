#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include "dictionary.h"
#include "hex_utils.h"
#include "debug.h"
#include "socket_io.h"
#define CHUNK_SIZE_BUFFER_SIZE 10


// read a request body using content-length if its available, or chunked 
// when we reach this function the input headers have already been sanity checked, i.e
// there's no need to look for degenerative cases such a no headers, conflicting headers, etc
int read_body(int in, dictionaryType *headers, unsigned char **body_data, int max_size, int *received_size ) {

    char chunk_size_hex[CHUNK_SIZE_BUFFER_SIZE];
    int chunk_length = 0;
    int content_length = 0;
    int total_read;
    int read_length;
    int chunk_length_so_far;
    int ret;
    int memory_size = 1024;
	char *content_length_header;
    char temp_buff[5];

 	content_length_header = findDict(headers, "Content-Length");

    if ( content_length_header ) {

        content_length = atoi( content_length_header );

        if ( content_length > max_size ) {

            debug( "This entity body exceeds the max allowed size\n");
            return 400;
        }

        // allocated an extra byte for a null in case this is ascii data we want to NULL terminate
        *body_data = malloc( content_length + 1 );

        if (*body_data == 0) {

            return 500;
        }

        // debug( "Reading %d bytes from the stream\n", content_length );

        total_read = 0;

        while(total_read != content_length) {

            read_length = read( in, (*body_data)+total_read, content_length - total_read );

            switch (read_length) {

                case -1:

                    debug("Read error on socket\n");
                    return 400;
                    break;

                case 0:

                    if ( total_read != content_length ) {

                        debug("Connection closed unexpectedly\n");
                        free(*body_data);
                        (*body_data) = 0;
                        return 400;
                    }

                    break;

                default:

                    total_read += read_length;
                    break;
            }

        }

        // null terminate the data 
        (*body_data)[content_length] = 0;
        
        *received_size = content_length;

        return 200;

    }
    else {

        total_read = 0;

        *body_data = calloc( 1, memory_size );

        if (*body_data == 0 ) {

            debug( "Unable to get memory\n");
            return 500;
        }

        while( 1 ) {

            ret = readLine(in, chunk_size_hex, CHUNK_SIZE_BUFFER_SIZE-1);

            if (ret == -1) {

                debug( "Line too long!\n");
                return 414;
            }

            chunk_length = chunkHexToInt(chunk_size_hex);

            if ( chunk_length == 0 ) {

                break;
            }

            if ( chunk_length + total_read +1 > memory_size ) {

                if ( memory_size + chunk_length +1 > max_size ) {

                    debug( "The current chunk will exceed the max entity size\n");
                    return 400;
                }

                *body_data = realloc( *body_data, memory_size + chunk_length + 1 );

                if (*body_data == 0 ) {

                    return 500;
                }

                memory_size += chunk_length + 1;

            }

            chunk_length_so_far = 0;

            while(chunk_length_so_far != chunk_length) {

                read_length = read( in, (*body_data)+total_read, chunk_length - chunk_length_so_far );

                // debug( "Read a chunk of size %d\n", read_length);

                switch (read_length) {

                    case -1:

                        debug("Read error on socket\n");
                        return 400;
                        break;

                    case 0:

                        if ( chunk_length_so_far != chunk_length ) {

                            debug("Connection closed unexpectedly\n");
                            free(*body_data);
                            (*body_data) = 0;
                            return 400;
                        }

                        break;

                    default:

                        total_read += read_length;
                        chunk_length_so_far += read_length;
                        break;
                }

            }

            readLine(in, temp_buff, 2);

            if ( ( temp_buff[0] != '\n') && ( temp_buff[0] != '\r' || temp_buff[1] != '\n') ) {

                debug( "Line not terminated properly\n");

                return 400;
            }

        }

        readLine(in, temp_buff, 4);

        // debug("memory_size = %d\n, total_data = %d\n", memory_size, total_read);

        (*body_data)[total_read] = 0;

        *received_size = total_read;

    }

    return 200;

}
