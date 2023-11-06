


int ishex( char x ) {

    if ( x >= '0' && x <= '9') 
        return 1;

    if (x >= 'A' && x <= 'F' ) 
        return 1;

    if (x >= 'a' && x <= 'f' )
        return 1;

    return 0;
}

char fromHex(char *hexdigits) {

    char value = 0;

    if ( hexdigits[0] >= '0' && hexdigits[0] <= '9' )
        value = (hexdigits[0] - '0' )*16;
    else if (hexdigits[0] >= 'A' && hexdigits[0] <= 'F') 
        value = ((hexdigits[0] - 'A') + 10) *16;
    else if (hexdigits[0] >= 'a' && hexdigits[0] <= 'f') 
        value = ((hexdigits[0] - 'a') + 10) *16;
    else
        return 0;

    if ( hexdigits[1] >= '0' && hexdigits[1] <= '9' )
        value += (hexdigits[1] - '0' );
    else if (hexdigits[1] >= 'A' && hexdigits[1] <= 'F') 
        value += ((hexdigits[1] - 'A') + 10);
    else if (hexdigits[1] >= 'a' && hexdigits[1] <= 'f') 
        value += ((hexdigits[1] - 'a') + 10);
    else
        return 0;     
    

    return value;
    
}

// very specific function to take a byte int and conver to 2 digit hex value
int toHex(unsigned char value, char *output) {

int digit;
char hexdigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    if ( value > 15 ) {

        digit = value / 16;

        output[0] = hexdigits[digit];

        digit = value % 16;

        output[1] = hexdigits[digit];

        output[2] = 0;
    }

    if ( value <= 15 ) {

        output[0] = hexdigits[value];
        output[1] = 0;

    }

    return 0;

}

int chunkHexToInt(char *hexdigits) {

int value = 0;

    while( ishex(*hexdigits ) ) {
        value *= 16;

        if ( *hexdigits >= '0' && *hexdigits <= '9' )
            value += (*hexdigits - '0' );
        else if (*hexdigits >= 'A' && *hexdigits <= 'F') 
            value += ((hexdigits[0] - 'A') + 10);
        else if (*hexdigits >= 'a' && *hexdigits <= 'f') 
            value += ((hexdigits[0] - 'a') + 10);

        ++hexdigits;

    }

    return value;
}
