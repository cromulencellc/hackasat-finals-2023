
#include "gtest/gtest.h"
#include "circular.hpp"

template<typename T> class CircularTest : public ::testing::Test
{
public:
    size_t n;
protected:
    CircularTest( )
    {
        n = N;
    }
    

    void fill( )
    {
        T t( 0 );
        for( size_t k = 0 ; k < N ; k++ )
        {
            this->c.push_back( t );
            t++;
        }
    }
    static const size_t N = 10;
    cromulence::buffer::circular< T , N > c;
};

typedef testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>  circ_test;
TYPED_TEST_SUITE(CircularTest , circ_test);

TYPED_TEST( CircularTest , PushToFull )
{
    this->fill();
    size_t sz;
    sz = this->c.size();
    EXPECT_EQ( sz , this->n );
    
}

TYPED_TEST( CircularTest , PopToEmpty )
{
    this->fill();
    size_t sz;
    sz = this->c.size();
    EXPECT_EQ( sz , this->n );
    for( size_t k=0; k < this->n; k++ )
    {
        this->c.pop_front( );
    }
    sz = this->c.size();
    EXPECT_EQ( sz, 0 );
}

TYPED_TEST( CircularTest , Clear )
{
    this->fill();
    size_t sz;
    sz = this->c.size();
    EXPECT_EQ( sz , this->n );
    this->c.clear();
    sz =  this->c.size();
    EXPECT_EQ( sz , 0 );
}

TYPED_TEST( CircularTest , Removal )
{
    this->fill();
    size_t sz;
    sz = this->c.size();
    EXPECT_EQ( sz , this->n );
    this->c.remove( 3  );
    sz =  this->c.size();
    EXPECT_EQ( sz , this->n -3  );
}
TYPED_TEST( CircularTest , MaxSizeReport )
{
    size_t sz= this->c.max_size();
    EXPECT_EQ( sz , this->n );
}
TYPED_TEST( CircularTest , Values )
{
    for( size_t k= 0; k < this->n ; k++ )
    {
        this->c.push_back( k );
    }
    for( size_t k= 0; k < this->n ; k++ )
    {
        TypeParam out;
        out = this->c.pop_front( );
        EXPECT_EQ( k , out );
    }
    
}

TYPED_TEST( CircularTest , Stradling )
{
    TypeParam number(0);
    for( size_t k=0 ; k < this->n/2 ; k++ )
    {
        
        this->c.push_back( number );
        number++;
    }
    for( size_t k=0; k < this->n/2 ; k++ )
    {
        auto out = this->c.pop_front();
        EXPECT_EQ( out , k );
    }
    // we should now have an empty buffer but the head should be 50% of the way through the space we've alloc'd
    EXPECT_EQ( 0 , this->c.size() );
    // Lets refill this thing
    TypeParam start = number;
    for( size_t k=0; k < this->n ; k++ )
    {
        this->c.push_back( number );
        number++;
    }
    // Now we should have a full buffer
    EXPECT_EQ( this->n , this->c.size());
    // Lets pop off the numbers and make sure we are ok;
    for( size_t k=0 ; k < this->n ;k++)
    {
        auto out = this->c.pop_front( );
        EXPECT_EQ( out , start + k );
    }
    // now it should be empty
    EXPECT_EQ( this->c.size() , 0 );
}
TYPED_TEST( CircularTest , ChunkyStradling )
{
    TypeParam number(0);
    for( size_t k=0 ; k < this->n/2 ; k++ )
    {
        
        this->c.push_back( number );
        number++;
    }
    for( size_t k=0; k < this->n/2 ; k++ )
    {
        auto out = this->c.pop_front();
        EXPECT_EQ( out , k );
    }
    // we should now have an empty buffer but the head should be 50% of the way through the space we've alloc'd
    EXPECT_EQ( 0 , this->c.size() );
    // Lets refill this thing but with a chunky push --- to enforce stradling
    TypeParam start = number;
    TypeParam data[this->n];
    for( size_t k=0; k<this->n;k++)
    {
        data[k]= number;
        number++;
    }
    this->c.push_back( data , this->n );
    
    // Now we should have a full buffer
    EXPECT_EQ( this->n , this->c.size());
    // Lets pop off the numbers and make sure we are ok;
    for( size_t k=0 ; k < this->n ;k++)
    {
        auto out = this->c.pop_front( );
        EXPECT_EQ( out , start + k );
    }
    // now it should be empty
    EXPECT_EQ( this->c.size() , 0 );
}

TYPED_TEST( CircularTest , ChunkyPoping )
{
    TypeParam number(0);
    for( size_t k=0 ; k < this->n/2 ; k++ )
    {
        
        this->c.push_back( number );
        number++;
    }
    for( size_t k=0; k < this->n/2 ; k++ )
    {
        auto out = this->c.pop_front();
        EXPECT_EQ( out , k );
    }
    // we should now have an empty buffer but the head should be 50% of the way through the space we've alloc'd
    EXPECT_EQ( 0 , this->c.size() );
    // Lets refill this thing but with a chunky push --- to enforce stradling
    TypeParam data[this->n];
    for( size_t k=0; k<this->n;k++)
    {
        data[k]= number;
        number++;
    }
    this->c.push_back( data , this->n );
    
    // Now we should have a full buffer
    EXPECT_EQ( this->n , this->c.size());
    // Lets pop off chunkily and make sure we are good
    TypeParam outData[this->n];
    this->c.pop_front( outData , this->n);
    for( size_t k=0 ; k < this->n ;k++)
    {
        EXPECT_EQ( data[k] ,outData[k] );
    }
    // now it should be empty
    EXPECT_EQ( this->c.size() , 0 );
}

TYPED_TEST( CircularTest , ThrowOnOverflow )
{
    this->fill();
    try
    {
        this->c.push_back( 0 );
        // we should never get here
        EXPECT_TRUE( false );
    }
    catch(const std::overflow_error& e)
    {
        EXPECT_TRUE( true );
    }
    catch( ... )
    {
        // we threw the wrong erroz
        EXPECT_TRUE( false );
    }
    
}
TYPED_TEST( CircularTest , ThrowOnUnderflow )
{
    this->fill();
    for( size_t k =0 ; k < this->n ;k++ )
    {
        this->c.pop_front();
    }
    try
    {
        this->c.pop_front( );
        // we should never get here
        EXPECT_TRUE( false );
    }
    catch(const std::underflow_error& e)
    {
        EXPECT_TRUE( true );
    }
    catch( ... )
    {
        // we threw the wrong erroz
        EXPECT_TRUE( false );
    }
    
}

TYPED_TEST( CircularTest , PatternFound )
{
    // test patterns in all locations
    bool found;
    this->fill();
    size_t patternLen(4);
    for( size_t k= 0 ; k < (this->c.max_size() - patternLen) ; k++)
    {
        TypeParam pattern[patternLen];
        for( size_t z =0 ; z < patternLen ; z++ )
        {
            pattern[z] = z + k;
        }
        size_t loc;
        found = this->c.find_pattern( pattern , patternLen ,loc );
        EXPECT_TRUE( found  );
        EXPECT_EQ( loc , k );
    }
    
}

TYPED_TEST( CircularTest , PatternNotFound )
{
    bool found;
    this->fill();
    TypeParam pattern[] = { 11,12,13,14};
    size_t loc;
    found = this->c.find_pattern( pattern , 4 ,loc );
    EXPECT_FALSE( found  );
}
TYPED_TEST( CircularTest , PartialPatterns )
{
    bool found;
    this->fill();
    TypeParam pattern[] = { 8,9,10,11};
    size_t loc;
    found = this->c.find_pattern( pattern , 4 ,loc );
    EXPECT_FALSE( found  );
}