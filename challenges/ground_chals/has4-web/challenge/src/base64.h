
#ifndef BASE64_H
#define BASE64_H

char *encode_base64(char *string_to_encode);
char *decode_base64(char *string_to_decode);
char *convert_to_binary(char a, char *binary, int binary_len);

#endif

