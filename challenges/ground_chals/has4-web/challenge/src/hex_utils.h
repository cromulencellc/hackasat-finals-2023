
#ifndef hex_utils_h

#define hex_utils_h


int ishex( char x );

char fromHex(char *hexdigits);

// very specific function to take a byte int and conver to 2 digit hex value
int toHex(unsigned char value, char *output);

// a more generic function to convert hex digits to an int value
int chunkHexToInt(char *hexdigits);

#endif