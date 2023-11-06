#pragma once
#include <stdio.h>
#include <filesystem>
namespace cromulence::buffer
{

template<typename T> class filestore
{
    public:
        filestore( std::string path ) : path_(path)
        {
            FILE *fp;
            if( std::filesystem::exists( path_ ) )
            {
                fp = fopen( path_.c_str() , "a+");
            
            }
            else
            {
                // otherwise just open er up as write
                fp = fopen( path_.c_str() , "w");
            }
            if( fp != NULL )
            {
                fclose(fp);
            }
            

        }
        ~filestore()
        {

        }
        void load( std::string path, T* out,  size_t maxcount)
        {
            size_t s;
            s = size();
            if(  maxcount < s)
            {
                throw std::range_error("filestore load - buffer too smol");
            }
            FILE *fp;
            fp = fopen( path.c_str() , "rb");
            if (fp == NULL) 
            {
                throw std::runtime_error("file pointer is null");
            }
            fread( out , sizeof(T) , s , fp);
            fclose(fp);
        }
        size_t size( )
        {
            size_t bytes = std::filesystem::file_size( path_ );
            size_t n;
            n = bytes / sizeof(T);
            return n; 
        }
        void clear()
        {
            if( std::filesystem::exists( path_ ) ) 
            {
                std::filesystem::remove( path_ );
            }
            FILE *fp;
            fp = fopen( path_.c_str() , "a");
            if( fp != NULL )
            {
                fclose(fp);
            }

        }
        void save( std::string newpath )
        {
            std::filesystem::copy( path_ , newpath );
        }
        void push_back( T &data)
        {
            push_back( &data  , 1 );
        }
        std::string getPath( )
        {
            return path_;
        }
        void push_back( const T *ptr , size_t count )
        {
            FILE *fp;
            fp = fopen( path_.c_str() , "a");
            size_t out;
            if (fp == NULL) 
            {
                throw std::runtime_error("file pointer is null");
            }
            out = fwrite( ptr , sizeof(T), count , fp );
            if( out != count )
            {
                //throw
            }
            fclose( fp );

        }
    protected:
        std::string path_;
};

}
