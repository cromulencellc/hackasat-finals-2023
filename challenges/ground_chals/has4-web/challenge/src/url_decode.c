#include <malloc.h>
#include <string.h>
#include "hex_utils.h"


char *url_decode(char *url) {

    int incoming_length;
    char *decoded_url;
    int index;
    int outindex = 0;

    if (!url) {

        return url;
    }

    incoming_length = strlen(url)+1;

    // a decoded URL will only be shorter, not longer than the incoming
    // so this is a safe amount of memory
    decoded_url = malloc(incoming_length);

    if (!decoded_url) {

        return decoded_url;
    }
    
    for (index=0; index < incoming_length; ++index) {

        if (url[index] == '%') {

            if ( incoming_length - index > 3 ) {

                if ( ishex(url[index+1]) ) {

                    if ( ishex(url[index+2]) ) {

                        decoded_url[outindex] = fromHex( url+index+1);
                        index += 2;
                    }
                }
            }
        }
        else {
            decoded_url[outindex] = url[index];
        }

        ++outindex;
    }
    
    return decoded_url;
}

char *form_decode(char *form_data) {

    int incoming_length;
    char *decoded_data;
    int index;
    int outindex = 0;

    if (!form_data) {

        return form_data;
    }

    incoming_length = strlen(form_data)+1;

    decoded_data = malloc(incoming_length+1);

    if (!decoded_data) {

        return decoded_data;
    }
    

    for (index=0; index < incoming_length; ++index) {

        if (form_data[index] == '+') {

            decoded_data[outindex] = ' ';
        }
        else if (form_data[index] == '%') {

            if ( incoming_length - index > 3 ) {

                if ( ishex(form_data[index+1]) ) {

                    if ( ishex(form_data[index+2]) ) {

                        decoded_data[outindex] = fromHex( form_data+index+1);
                        index += 2;
                    }
                }
            }
        }
        else {
            decoded_data[outindex] = form_data[index];
        }
        
        ++outindex;
    }

    decoded_data[outindex] = 0;
    return decoded_data;
}
