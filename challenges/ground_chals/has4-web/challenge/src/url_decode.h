#ifndef url_decode_h

#define url_decode_h

char *url_decode(char *url);
char *handle_binary_data( unsigned char *data, unsigned int size);
char *handle_text_data( char *data, unsigned int size);


#endif