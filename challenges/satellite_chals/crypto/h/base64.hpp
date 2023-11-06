#pragma once


#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
namespace cromulence::crypto
{
    namespace {
struct BIOFreeAll { void operator()(BIO* p) { BIO_free_all(p); } };
}

    inline size_t decoded_length(const char* b64input)
    { //Calculates the length of a decoded string
        size_t len = strlen(b64input);
        size_t padding = 0;

        if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
            padding = 2;
        else if (b64input[len-1] == '=') //last char is =
            padding = 1;

        return (len*3)/4 - padding;
    }
    inline unsigned char* base64_decode( void *src, size_t sizeBytes , size_t &newSize)
    {
        const char *encoded = reinterpret_cast<const char*>( src );

        std::unique_ptr<BIO,BIOFreeAll> b64(BIO_new(BIO_f_base64()));
        BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
        BIO* source = BIO_new_mem_buf(encoded, -1); // read-only source
        BIO_push(b64.get(), source);
        const int maxlen = strlen(encoded) / 4 * 3 + 1;
        unsigned char* decoded = new unsigned char[ maxlen];
        const int len = BIO_read(b64.get(), decoded, maxlen);
        newSize = len;
        return decoded;
    }
    inline std::string base64_encode( void *src, size_t  sizeBytes ) 
    {
        const char *toEncode = reinterpret_cast<const char*>( src );
        std::unique_ptr<BIO,BIOFreeAll> b64(BIO_new(BIO_f_base64()));
        BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
        BIO* sink = BIO_new(BIO_s_mem());
        BIO_push(b64.get(), sink);
        BIO_write(b64.get(),toEncode, sizeBytes);
        BIO_flush(b64.get());
        const char* encoded;
        const long len = BIO_get_mem_data(sink, &encoded);
        return std::string(encoded, len);;
    }

}