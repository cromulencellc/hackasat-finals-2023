
#include "gtest/gtest.h"
#include "filestore.hpp"

template<typename T> class FileStoreTest : public ::testing::Test
{
public:

    void fill( int N )
    {
        T t( 0 );
        for( size_t k = 0 ; k < N ; k++ )
        {
            this->c.push_back( t );
            t++;
        }
    }
protected:
    FileStoreTest( ) :
        path_("teststore.gg"), 
        c( path_ )
    {
    }
    void SetUp()
    {
        if( std::filesystem::exists( path_ ))
        {
            std::filesystem::remove( path_ );
        }
        if( std::filesystem::exists( "whatever.gg"))
        {
            std::filesystem::remove("whatever.gg");
        }

    }
    void TearDown()
    {
        
    }

    std::string path_;
    cromulence::buffer::filestore< T > c;
};

typedef testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>  circ_test;
TYPED_TEST_SUITE(FileStoreTest , circ_test);

TYPED_TEST( FileStoreTest , Appendage )
{
    this->fill(100);
    EXPECT_EQ( this->c.size() , 100 ) ;
    this->fill(100);
    EXPECT_EQ(this->c.size() , 200 );
}

TYPED_TEST( FileStoreTest , Clear )
{
    this->fill(100);
    EXPECT_EQ( this->c.size() ,100);
    this->c.clear();
    EXPECT_EQ( 0 , this->c.size() );
    this->fill(100);
    EXPECT_EQ( this->c.size() , 100 );
}

TYPED_TEST( FileStoreTest , Save )
{
    this->fill(200);
    EXPECT_EQ( this->c.size( ), 200 );
    this->c.save("whatever.gg");
    this->c.clear();
    EXPECT_EQ( this->c.size( ) , 0 );
    EXPECT_EQ( std::filesystem::file_size("whatever.gg"), 200*sizeof(TypeParam));
    std::filesystem::remove( "whatever.gg");
}

TYPED_TEST( FileStoreTest , Load )
{
    this->fill( 200 );
    this->c.save( "whatever.gg");
    this->c.clear();
    EXPECT_EQ( this->c.size( ) , 0 );
    TypeParam o[200];
    this->c.load( "whatever.gg", o, 200);    
}
TYPED_TEST( FileStoreTest , OOBLoad )
{
this->fill( 200 );
    this->c.save( "whatever.gg");
    this->c.clear();
    EXPECT_EQ( this->c.size( ) , 0 );
    TypeParam o[199];
    this->c.load( "whatever.gg", o, 199);    
}