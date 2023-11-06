#pragma once

#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <system_error>
#include <stdexcept>
#include <cerrno>
#include <sys/ioctl.h>
#include <sys/stat.h>
 #include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <fstream>
namespace cromulence::buffer
{   
    enum RW
    {
        READ=  0,
        WRITE
    };
    template< typename T > class persistent
    {
    public:
        persistent( const char *path ,  RW type , size_t maxSizeApprox  ) :
            path_( path ), 
            maxSizeAsked_( maxSizeApprox ),
            type_(type)
        {
            open_fifo( type );
            
        }
        ~persistent()
        {
            close( fd_ );
            
            
        }
        void open_fifo( RW type )
        {
            namespace fs = std::filesystem;

            int creation;
             std::filesystem::path p( path_.c_str() );
            // check for path existence - if it doesnt exist create it
            bool e =  fs::exists( p.parent_path() );
            if( false == e  )
            {
                 fs::create_directories( p.parent_path() );
            }
            //create the fifo if needed
            creation = mkfifo( path_.c_str() , 0666 );
            bool created;
            created = ( creation ==0 ) || ( errno == EEXIST );
            if( false == created ) 
            {
                // TODO: Why is the fifo throwing -1 if it exists instead of -17 (EEXIST)
                throw std::system_error(std::error_code(), "persistent: fifo not available");
            }
            
            int rw = (type == READ) ? O_RDONLY : O_RDWR;
            //open teh pipe
            size_t maxSizeBytes = maxSizeAsked_ * sizeof(T);
            fd_ = open( path_.c_str() , rw | O_NONBLOCK);
            if( fd_ == -1 )
            {
                throw std::system_error( std::error_code(), "persistent: cant open");
            }
            // Only readers can set the pipe size
            if( type == READ )
            {
                auto success = fcntl( fd_ , F_SETPIPE_SZ , maxSizeBytes ) ;
                if( success == -1 )
                {
                    throw std::system_error( std::error_code(), "persistent: cant set pipe size?");
                }
            }
            //
            long pipe_size = (long)fcntl(fd_, F_GETPIPE_SZ);
            maxSizeBytes_ = static_cast< size_t >( pipe_size );
            maxSizeItems_ =  maxSizeBytes_ / sizeof(T);
            
            // stat the file
            update_size();
            if( (byteSize_ %= sizeof(T)) != 0 )
            {
                throw std::system_error(std::error_code(),  "persistent: inconsistent item size in fifo");
            }
            
        }
        void kill( )
        {
            // close our file descriptor
            close( fd_ );
            // unlink to remove the fifo
            unlink( path_.c_str() );
            //std::filesystem::remove( path_ );
        }
        void clear( )
        {
            update_size( );
            for( size_t k = 0 ; k < itemSize_; k++ )
            {
                pop_front();
            }
        }
        void save( std::string path )
        {
            
            update_size();
            size_t nItems(itemSize_);
            FILE *fp;
            T *data = new T[nItems];
            fp = fopen( path.c_str() , "wb");
            // pop all the data 
            pop_front( data , nItems );
            // write the data to the file
            fwrite( data , sizeof(T) , nItems , fp );
            fclose( fp );
            
            // but now we have eliminated the data so we have to restore lol
            push_back( data , nItems );
            delete[] data;
        }
        void restore( std::string path )
        {
            size_t nBytes;
            size_t nItems;
            FILE *fp;
            struct stat sb;
            stat(path.c_str(), &sb);
            nBytes = sb.st_size;
            if( nBytes % sizeof(T) == 0 )
            {
                // we gucci
                nItems = nBytes / sizeof( T );
            }
            else
            {
                throw std::runtime_error("persistent: restore size mismatch");
            }
            T *data = new T[nItems];

            fp = fopen( path.c_str() , "rb");
            // write the data to the file
            fread( data , sizeof(T) , nItems , fp );
            fclose( fp );
            // clear things out first - we dont want intermingled data
            clear();
            // but now we have eliminated the data so we have to restore lol
            push_back( data , nItems );
            delete[] data;
            update_size();
        }
        size_t size()
        {
            update_size();
            return itemSize_;
        }
        size_t max_size()
        {
            return maxSizeItems_;
        }
        void push_back( const T &obj )
        {
            push_back( &obj , 1);
        }
        void push_back(const T * ptr , size_t count )
        {
            ssize_t written;
            
            update_size();
            written = write( fd_  , ptr , count*sizeof(T));
            
            ssize_t n = static_cast<ssize_t>( count*sizeof(T));
            if( written != n )
            {
                throw std::overflow_error("persistent: overflow");
            }

        }
        T pop_front( )
        {
            T out;
            pop_front( &out , 1 );
            return out;
        }
        void pop_front( T* ptr ,size_t count )
        {
            ssize_t out;
            out = read( fd_ , ptr , count*sizeof(T));
            if( out != static_cast<ssize_t>( count*sizeof(T)) )
            {
                throw std::underflow_error("persistent: underflow");
            }
        }
        std::string getPath( )
        {
            return path_;
        }

    protected:
        void update_size( )
        {

            ioctl(fd_, FIONREAD, &byteSize_  );
            // use cached size
            itemSize_ = byteSize_ / sizeof(T);
        }
        std::string path_;
        size_t maxSizeBytes_;
        size_t maxSizeItems_;
        size_t maxSizeAsked_;
        size_t itemSize_;
        size_t byteSize_;
        RW type_;
        int fd_;
        
    };
}

