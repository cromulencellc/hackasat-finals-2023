#pragma once
#include "gtest/gtest.h"

#include "key_queue.hpp"

template<typename T> class KeyQueueTest : public ::testing::Test
{
public:
    size_t n;
protected:
    KeyQueueTest( )
    {
        n = N;
    }
    

    
    static const size_t N = 10;
    cromulence::key_queue<  std::string, T > q_;
};

template<typename T> bool is_one( T left )
{
    return left == 1;
}





typedef testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>  kq_test;
TYPED_TEST_SUITE(KeyQueueTest , kq_test);

TYPED_TEST( KeyQueueTest , PushOrder )
{
    this->q_.push( "a" , 1 );
    this->q_.push( "b",  2);
    this->q_.push( "c", 3 );
 
    EXPECT_EQ( this->q_.size()  , 3 );


   EXPECT_EQ( *this->q_.front(), 1);
    this->q_.pop();

    EXPECT_EQ( *this->q_.front(), 2);
    this->q_.pop();

    EXPECT_EQ( *this->q_.front(), 3);
    this->q_.pop();
}

TYPED_TEST( KeyQueueTest , RemoveByKey  )
{
    this->q_.push( "a" , 1 );
    this->q_.push( "b",  2);
    this->q_.push( "c", 3 );

    this->q_.remove( "b");

    EXPECT_EQ( *this->q_.front(), 1);
    this->q_.pop();

    EXPECT_EQ( *this->q_.front(), 3);
    this->q_.pop();

    EXPECT_EQ( this->q_.size() , 0 );
}


TYPED_TEST( KeyQueueTest , GetByKeyAndModify  )
{
    this->q_.push( "a" , 1 );
    this->q_.push( "b",  2);
    this->q_.push( "c", 3 );



    EXPECT_EQ( *this->q_.get("a"), 1);
    EXPECT_EQ( *this->q_.get("b"), 2);
    EXPECT_EQ( *this->q_.get("c"), 3);

    std::shared_ptr< TypeParam > o = this->q_.get("b");

    EXPECT_EQ( *o , 2 );
     
    *o=74;
    EXPECT_EQ( *this->q_.get("b"), 74);
}


TYPED_TEST( KeyQueueTest , GetByIndex  )
{
    this->q_.push( "a" , 1 );
    this->q_.push( "b",  2);
    this->q_.push( "c", 3 );



    EXPECT_EQ( this->q_.getKey(0), "a");
    EXPECT_EQ( this->q_.getKey(1), "b");
    EXPECT_EQ( this->q_.getKey(2), "c");

    std::shared_ptr< TypeParam > o = this->q_.get("b");

    EXPECT_EQ( *o , 2 );
     
    *o=74;
    EXPECT_EQ( *this->q_.get("b"), 74);
}



TEST( IntKeyQueue , FindFirst )
{
    cromulence::key_queue<std::string , int > qq;

    qq.push( "a",2) ;
    qq.push( "b",1) ;
    qq.push( "c",1) ;
    qq.push( "d",4) ;
    std::string o = qq.first_match< is_one<int> >();

    EXPECT_EQ( o,  "b");
    auto data = qq.get("b");
    *data= 74;

    std::shared_ptr<int> o2 = qq.get("b");
    EXPECT_EQ( *o2, 74 );
}

