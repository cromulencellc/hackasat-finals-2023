





      /* Make the table for a fast CRC. */
      void make_crc_table(unsigned long *crc_table)
      {
        unsigned long c;
        int n, k;
        for (n = 0; n < 256; n++) {
          c = (unsigned long) n;
          for (k = 0; k < 8; k++) {
            if (c & 1) {
              c = 0xedb88320L ^ (c >> 1);
            } else {
              c = c >> 1;
            }
          }
          crc_table[n] = c;
        }
      }

      /*
         Update a running crc with the bytes buf[0..len-1] and return
       the updated crc. The crc should be initialized to zero. Pre- and
       post-conditioning (one's complement) is performed within this
       function so it shouldn't be done by the caller. Usage example:

         unsigned long crc = 0L;

         while (read_buffer(buffer, length) != EOF) {
           crc = update_crc(crc, buffer, length);
         }
         if (crc != original_crc) error();
      */

      unsigned long crc32(unsigned long crc, unsigned char *buf, int len)
      {

        static int quick_table_done = 0;
        static unsigned long crc_table[256];

        unsigned long c = crc ^ 0xffffffffL;
        int n;

        if (!quick_table_done) {
          make_crc_table(crc_table);

            quick_table_done = 1;
        }

        for (n = 0; n < len; n++) {
          c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
        }
        return c ^ 0xffffffffL;
      }

      /* Return the CRC of the bytes buf[0..len-1]. */
    //   unsigned long crc(unsigned char *buf, int len)
    //   {
    //     return update_crc(0L, buf, len);
    //   }
