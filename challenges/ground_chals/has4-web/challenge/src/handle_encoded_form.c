#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "http.h"
#include "debug.h"


char *handle_encoded_form_data( char *data) {

char *decoded_data;
char *formatted_data;
int buffer_size = 1000;
int used_size = 0;
char *key;
char *value;


    if ( data == 0 ) {
        return 0;

    }

    decoded_data = form_decode( data );

    if ( decoded_data == 0 ) {

        return 0;
    }

    formatted_data = malloc(buffer_size);

    if ( formatted_data == 0 ) {

        return 0;
    }

    // debug( "%s\n", decoded_data);

    if ( strchr(data, '=') == 0 ) {

        debug( "This does not appear as key/value pair data\n");

        snprintf(formatted_data, buffer_size, "%s",
           "<html><body>This does not appear to be urlencoded data!</br></body></html>" );

        free(decoded_data);

        return formatted_data;
    }

    used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%s", 
        "<html><head><style>table, th, td { border: 1px solid black; }</style></head><body><table style=\"width:100%\"><tr><th>Key</th><th>Value</th></tr>");

    char *separator;
    int finished = 0;

    separator = strchr(decoded_data, '=');
    key = decoded_data;

    while (1) {

        if (separator == 0 )
            break;

        value = separator + 1;

        *separator = 0;

        separator = strchr(value, '&');

        if (!separator) {

            finished = 1;
        }
        else {

            *separator = 0;
        }

        // debug( "key = %s, value = %s\n", key, value);

        int added_size;

        added_size = snprintf( 0, 0, "<tr><td>%s</td><td>%s</td></tr>", key, value);

        // debug( "added_size = %d\n", added_size);

        if (used_size + added_size > buffer_size) {

            // debug( "allocating more memory\n");

            buffer_size += (added_size + 200);

            formatted_data = realloc(formatted_data, buffer_size);
        }

        if ( formatted_data == 0 ) {

            exit(-1);
        }

        used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "<tr><td>%s</td><td>%s</td></tr>",
                key, value);

        if (finished)
            break;

        key = separator + 1;
        separator = strchr(key, '=');

    }

    used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%s", "</table></body></html>");

    // debug("%s\n", formatted_data);

    free(decoded_data);

    return formatted_data;

}


char *handle_binary_data( unsigned char *data, unsigned int size) {

char *formatted_data;
int buffer_size = 0;
int used_size = 0;
int i;


    if ( data == 0  || size == 0 ) {

        return 0;
    }

    buffer_size = size * 4 + 200;

    formatted_data = malloc(buffer_size);

    if ( formatted_data == 0 ) {

        return 0;
    
    }

    used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%s", "<html><body>");

    for (i=0; i < size; ++i ) {

        used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%02x ", data[i]);

        if ( (i+1) % 16 == 0 ) {

            used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "</br>");

        }

    }

    used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%s", "</body></html>");

    if ( used_size > buffer_size ) {

        exit(-1);
    }

    // debug( "%s\n", formatted_data);

    return formatted_data;

}

char *handle_text_data( char *data, unsigned int size) {

char *formatted_data;
int buffer_size = 0;
int used_size = 0;
int i;

    if ( data == 0 || size == 0 ) {

        return 0;
    }

    buffer_size = size * 4 + 200;

    formatted_data = malloc(buffer_size);

    if ( formatted_data == 0 ) {

        return 0;
    }

    used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%s", "<html><body>");

    for (i=0; i < size; ++i ) {

        used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%02x ", data[i]);

        if ( (i+1) % 16 == 0 ) {

            used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "</br>");

        }

    }

    used_size += snprintf(formatted_data+used_size, buffer_size - used_size, "%s", "</body></html>");

    if ( used_size > buffer_size ) {

        exit(-1);
    }
    
    // debug( "%s\n", formatted_data);

    return formatted_data;

}
