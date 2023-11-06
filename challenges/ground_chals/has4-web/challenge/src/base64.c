#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "base64.h"
#include "debug.h"


static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};


char *decode_base64(char *string_to_decode){

   int i = 0;
   int z = 0;
   int y = 0;
   int v = 0;
   int equals_count = 0;

   char *decoded = calloc(1, 1369);
   char *six_hold = calloc(1, 1024);

   if ( string_to_decode == 0 )
      return 0;

   if( strlen(string_to_decode) % 4 != 0 ) {

      debug( "The strlen() of the %s you are attempting to decode is: %ld\n" , string_to_decode, strlen(string_to_decode));
      debug( "The string you are trying to decode is not properly encoded\n");
      return NULL;

   }

   for(v = 0; v < strlen(string_to_decode); ++v) {

      if(string_to_decode[v] == '=') {

         equals_count++;
      }

   }

   for(z = 0; z < strlen(string_to_decode); z += 4) {

      //find corresponding base64 values from encoding tables
      int j = 0;
      char b64_values[100][100];

      memset(b64_values, 0, sizeof(b64_values));

      for(i = z , y = 0; i < z + 4; y++, i++){

         for(j = 0; j < sizeof(encoding_table); ++j){

            if(encoding_table[j] == string_to_decode[i]) {

               snprintf(b64_values[y] , 3, "%d", j);

            }

            if(j == strlen(encoding_table - 1)) {

               //strncpy(b64_values[y], "0" , 1);
               memcpy(b64_values[y], "0" , 1);
               b64_values[y][1] = '\0';

            }

         } 

      }

      //convert those to binary
      char six_bit_values[100][100];

      memset(six_bit_values, 0, sizeof(six_bit_values));

      int k = 0;

      for(k = 0; k < 4; ++k){

            six_hold = convert_to_binary(atoi(b64_values[k]), six_hold, 6);
            strncpy(six_bit_values[k], six_hold , 6);
            six_bit_values[k][6] = '\0';

      }

      //Concat the four binary values into one string
      char intermed_binary[2800];

      memset(intermed_binary, 0 , 2800);
      strncpy(intermed_binary, six_bit_values[0], 7);
      strncat(intermed_binary, six_bit_values[1], 7);
      strncat(intermed_binary, six_bit_values[2], 7);
      strncat(intermed_binary, six_bit_values[3], 7);

      //Split the concat in to 3 8 bit slices
      char eight_bit_values[500][500];

      memset(eight_bit_values, 0, sizeof(eight_bit_values));
      int p = 0;
      int q = 0;

      for(p = 0; p < 3; ++p){

         for(q = 0; q < 8; ++q){

            eight_bit_values[p][q] = intermed_binary[(p * 8) + q];  
         }

      }

      //Add up the 8 binary values to dec
      int m = 0;
      int n = 0;
      int power = 0;
      int eight_bit_total = 0;

      char ascii_vals[50][50];

      memset(ascii_vals, 0 , sizeof(ascii_vals));

      for(m = 0; m < 3; ++m) {

         for(n = 7; n >= 0 ; --n) {

            if (eight_bit_values[m][n] == '1') {

               eight_bit_total += pow(2, power);

            }

            power++;
         }

         snprintf(ascii_vals[m] , 4, "%d" , eight_bit_total);

         eight_bit_total = 0;
         power = 0;
      }

      //Combine the characters to their corresponding ascii values
      if(z == 0){

         snprintf(decoded, sizeof(int)  * 3 , "%c%c%c" , atoi(ascii_vals[0]) ,atoi(ascii_vals[1]), atoi(ascii_vals[2]));

      } 
      else {

         char *new_decoding = calloc(1,1024);

         snprintf(new_decoding, strlen(ascii_vals[0]) + strlen(ascii_vals[1]) + strlen(ascii_vals[2]), "%c%c%c" ,atoi(ascii_vals[0]) ,atoi(ascii_vals[1]), atoi(ascii_vals[2]));
         
         strncat(decoded, new_decoding, 1024);

         free(new_decoding);
         new_decoding = 0;

      }

   }

   free(six_hold);
   six_hold = NULL;

   return decoded;

}

//base 10 number in string form
char *convert_to_binary(char a , char *binary, int binary_len){

   int i = 0;
   int n = a;

   if ( binary == 0 ) 
      return 0;

   for(i = binary_len - 1; i >= 0; --i) {

      binary[i] = 48 + (n % 2);
      n  =  n / 2;

   }

   return binary;

}
