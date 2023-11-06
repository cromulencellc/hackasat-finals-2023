
#include "gtest/gtest.h"
#include "persistent.hpp"
#include <limits>
using persistent_type_list = testing::Types<uint8_t, uint16_t,uint32_t, uint64_t>;



template<typename T> class PersistentTest : public ::testing::Test
{
public:
    size_t n;
protected:
    PersistentTest() : 
        out("./test", cromulence::buffer::READ, N )
        ,in("./test" , cromulence::buffer::WRITE, N)

    {
        n =  N;
        max = std::numeric_limits<T >::max();

    }
    ~PersistentTest()
    {
        in.kill();
        out.kill();
    }
    virtual void TearDown( )
    {

    }

    void fill( )
    {
        T t( 0 );
        size_t maxCount = in.max_size();
        for( size_t k = 0 ; k < maxCount ; k++ )
        {
            this->in.push_back( t );
            t++;
        }
    }
    static const size_t N = 32000 ;    
    T max;
    cromulence::buffer::persistent< T  > out;

    cromulence::buffer::persistent< T > in;

};
TYPED_TEST_CASE(PersistentTest , persistent_type_list);

TYPED_TEST( PersistentTest , PushToFull )
{
    this->fill();
    size_t sz1,sz2;
    sz1 = this->in.size();
    sz2 = this->out.size();

    EXPECT_GE( sz1 , this->n );
    EXPECT_EQ( sz2 , sz1 );
    
}
TYPED_TEST( PersistentTest , Values )
{


    for( size_t k= 0; k < this->n ; k++ )
    {
        TypeParam i = k % this->max;
        this->in.push_back( i );
    }
    for( size_t k= 0; k < this->n ; k++ )
    {
        TypeParam o;
        TypeParam expected = k % this->max;
        o = this->out.pop_front( );
        EXPECT_EQ( expected , o );
    }
    
}
TYPED_TEST( PersistentTest , Sizes )
{


    for( size_t k= 0; k < this->n ; k++ )
    {
        TypeParam i = k % this->max;
        this->in.push_back( i );
        EXPECT_EQ( k+1 ,  this->in.size() );
    }
    for( size_t k= 0; k < this->n ; k++ )
    {
        TypeParam o;
        TypeParam expected = k % this->max;
        o = this->out.pop_front( );
        EXPECT_EQ( expected , o );
        size_t s = this->n - k-1;
        EXPECT_EQ( this->in.size() , s );
    }
    
}
TYPED_TEST( PersistentTest , PopToEmpty )
{
    this->fill();
    size_t sz;
    sz = this->out.size();
    EXPECT_GE( sz , this->n );
    for( size_t k=0; k < sz; k++ )
    {
        this->out.pop_front( );
    }
    sz = this->out.size();
    EXPECT_EQ( sz, 0 );
    sz = this->in.size();
    EXPECT_EQ( sz, 0 );
    
}

TYPED_TEST( PersistentTest , Clear )
{
    this->fill();
    size_t sz1,sz2;
    sz1 = this->in.size();
    sz2 = this->out.size();
    EXPECT_EQ( sz1 , this->in.max_size() );
    EXPECT_EQ( sz2 , this->out.max_size() );
    this->in.clear();
    //this->out.clear();
    sz1 =  this->in.size();
    sz2 = this->out.size();
    EXPECT_EQ( sz1 , 0 );
    EXPECT_EQ( sz2 , 0 );

    this->in.push_back( 2 );
    this->in.push_back( 3 );
    this->in.push_back( 4 );
    sz2 = this->out.size();
    sz1 = this->in.size();
    EXPECT_EQ( sz1 , 3 );
    EXPECT_EQ( sz2 , 3 );

}
TYPED_TEST( PersistentTest , Recover )
{
    size_t N = 100;
    namespace cb = cromulence::buffer;
    size_t sz1,sz2;
    cb::persistent< TypeParam > *i,*o;
    o = new cb::persistent< TypeParam >("./test111", cb::READ , 10000);
    //o1 = new cb::persistent< TypeParam >("./test111", cb::READ );
    i = new cb::persistent< TypeParam >("./test111", cb::WRITE, 0);
    // fill N items
    for( size_t k = 0 ; k < N ; k ++)
    {
        i->push_back( k );
    }
    // Lets save from the input side
    i->save("./restorePoint");
    
    sz1 = i->size();
    sz2 = o->size();
    // Make sure save didnt bungle our datas size
    EXPECT_EQ( sz1, N );
    EXPECT_EQ( sz2 , N );
    // Make sure save didnt bungle our datas order
    for( size_t k=0; k<N;k++)
    {
        auto z = o->pop_front();
        EXPECT_EQ( z , k );
    }
    // Lets destroy these 
    delete i; 
    delete o;
    // Now repoen them 
    o = new cb::persistent< TypeParam >("./test111", cb::READ,10000);
    i = new cb::persistent< TypeParam >("./test111",cb::WRITE,0);
    sz1 = i->size();
    sz2 = o->size();
    // They should be empty 
    EXPECT_EQ( sz1 , 0 );
    EXPECT_EQ( sz2 , 0 ) ;
    // Now restore them from the input side
    i->restore("./restorePoint");
    sz1 = i->size();
    sz2 = o->size();
    // did it restore the right number of items?
    EXPECT_EQ( sz1 , N );
    EXPECT_EQ( sz2 , N ) ;
    // Make sure save didnt bungle our datas order on restore
    for( size_t k=0; k<N;k++)
    {
        auto z = o->pop_front();
        EXPECT_EQ( z , k );
    }
}



TYPED_TEST( PersistentTest , MaxSizeReport )
{
    size_t sz= this->in.max_size();
    // Max size must be BIGGER than what we  asked fo
    EXPECT_GE( sz , this->n );
}

TYPED_TEST( PersistentTest , ThrowOnOverflow )
{
    this->fill();
    
    EXPECT_ANY_THROW( this->in.push_back( 0 ) ) ;
    
}
TYPED_TEST( PersistentTest , ThrowOnUnderflow )
{
    this->fill();
    auto S = this->out.size();
    for( size_t k =0 ; k < S ;k++ )
    {
        this->out.pop_front();
    }
    
    auto sz = this->out.size();
    EXPECT_EQ( 0 , sz );
    EXPECT_ANY_THROW( this->out.pop_front( )  );
    
    
}