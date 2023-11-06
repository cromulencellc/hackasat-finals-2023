#pragma once

#include <openssl/md5.h>
namespace cromulence::crypto
{
    class MD5
    {
    public: 
        MD5( )
        {
            MD5_Init(&md5_);
        }
        ~MD5( )
        {

        }
        void update( const void *bytes , size_t n )
        {
            MD5_Update(&md5_, bytes, n);

        }
        unsigned char* digest( )
        {
            MD5_Final( hash , &md5_);
            return hash;
        }
        size_t hash_size()
        {
            return MD5_DIGEST_LENGTH;
        }
    protected:
        MD5_CTX md5_;
        unsigned char hash[MD5_DIGEST_LENGTH ];
    };
}