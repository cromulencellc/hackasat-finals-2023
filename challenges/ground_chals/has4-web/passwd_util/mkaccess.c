#include <stdio.h>
#include <crypt.h>
#include <string.h>

#define MAX_PASSWORD 12

int main(void) {

char passwd1[MAX_PASSWORD];

    printf("Enter the password: ");
    fgets(passwd1, MAX_PASSWORD, stdin);


    passwd1[strlen(passwd1)-1] = 0;

    char *encryptedPassword = crypt(passwd1, "i7");

    printf("%s\n", encryptedPassword);


}


