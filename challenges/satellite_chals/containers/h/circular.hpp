#pragma once

#include <string>

#include <stdexcept>
#include <cstdint>
#include <cstring>
namespace cromulence::buffer
{

    template< typename T , size_t N> class circular
    {
    public:
        circular( ) :
            front_(0),
            back_(0),
            available_(N),
            fullness_(0)
        {

        }
        ~circular( )
        {

        }
        void clear()
        {
            memset( buffer_ , 0 , N*sizeof(T) );
            front_ = 0;
            back_ = 0;
            available_ = N;
            fullness_ = 0;
        }
        size_t size( )
        {
            return fullness_;
        }
        size_t max_size( )
        {
            return N;
        }
        size_t available( )
        {
            return N-fullness_;
        }
        void push_back( const T &obj )
        {
            push_back( &obj , 1 );
        }
        void push_back( const T *ptr , size_t count )
        {
            // case 1: [free space] [ old data ] [ new data][ free space ]
            // case 2: [new data ][ free space ][old data]
            // case 3: [ new data ] [ free space ][ old data][ new data ]
            // Do we have enough space
            if( available_ < count  )
            {
                throw std::overflow_error("circular");
            }
            if ( back_+count > N )
            {
                // case 3
                size_t sz1;
                size_t sz2;
                
                sz1 = ( N - back_ );
                sz2 = ( count - sz1 );
                std::memcpy( &buffer_[back_] , ptr , sz1*sizeof(T) );
                std::memcpy( &buffer_[0] , ptr + sz1  , sz2*sizeof(T));

            }
            else
            {
                // case 1
                // case 2
                std::memcpy( &buffer_[back_] , ptr , count*sizeof(T));
            }

            available_ -= count;
            fullness_ += count;
            back_ += count;
            back_ %= N;

        }
        T pop_front( )
        {
            T out;
            pop_front( &out , 1 );
            return out;
        }
        void pop_front( T* dest , size_t count )
        {
            if( fullness_ < count )
            {
                throw std::underflow_error("circular");
            }

            if(  count > (N-front_) )
            {
                size_t sz1,sz2;
                sz1 = N - front_;
                sz2 = count - sz1;
                std::memcpy( dest , &buffer_[front_] , sz1 *sizeof(T));
                std::memcpy( dest+sz1 , &buffer_[0] , sz2 *sizeof(T));
            }
            else
            {
                std::memcpy( dest , &buffer_[front_] , count*sizeof(T) );
            }
            remove( count );
        }
        void remove( size_t count )
        {
            count = ( count < fullness_) ? count : fullness_; // clamp
            front_ += count;
            front_ %= N;
            fullness_ -= count;
            available_ += count;
        }
        bool find_pattern( const T pattern[] , size_t M , size_t &out )
        {
            if( M > fullness_ )
            {
                out = 0;
                return false;
            }
            size_t stop( fullness_-M);
            for( size_t k= 0 ; k < stop ; k++  )
            {
                bool match(true);
                // 
                for( size_t z = 0 ; z <M ; z++ )
                {
                    size_t loc = (front_ + k + z)  % N;
                    match &=( pattern[z] == buffer_[loc] ) ;
                    
                }
                if( true == match )
                {
                    out = k;
                    return true;
                }
            }
            out = (stop>1) ? stop-1: 0 ;
            return false;
        }
        void print(  )
        {
            printf("Buffer is size %d: ", size() ) ;
            for( size_t k=0; k < size(); k++ )
            {
                size_t idx = ( front_ + k ) % N;  
                printf("%x " , buffer_[idx]);
            }
            printf("\n");
        }
    protected:
        size_t front_;
        size_t back_;
        size_t available_;
        size_t fullness_;
        T buffer_[N];
    };
}

