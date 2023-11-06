#include "gtest/gtest.h"
#include <thread>
#include <chrono>
#include "AttitudeMessages.hpp"
#include "HeartbeatMessages.hpp"
#include "IoMessages.hpp"
#include "AppTester.hpp"
#include <filesystem>
class AttitudeAppTest : public ::testing::Test
{
public:


protected:
    AttitudeAppTest( ) :
        testQ_( "atttest", 10 )
    {
    }
        void SetUp()
    {
        namespace A = cromulence::attitude;
        testQ_.subscribe<A::HousekeepingMsg>( );
        testQ_.subscribe<moonlighter::io::AcbPointMsg>();
        testQ_.subscribe<moonlighter::io::AcbGetTlmMsg>();

        A::ResetMsg r;
        r.send();
    }
    void TearDown()
    {
       
    }
    cromulence::TestMsgQueue testQ_;

};

using namespace std::chrono_literals;
TEST_F( AttitudeAppTest , NoOp  )
{

    cromulence::attitude::NoOpMsg nop;
    nop.send();
    testQ_.EXPECT_NO_MSG(1s);
}
TEST_F( AttitudeAppTest , Reset  )
{

    cromulence::attitude::ResetMsg reset;
    reset.send();
    testQ_.EXPECT_NO_MSG(1s);
}

TEST_F( AttitudeAppTest , Polling  )
{
    cromulence::attitude::ConfigMsg c;
    c.data.payload.enabled = 1;
    c.data.payload.frame_count = 10;
    c.data.payload.max_blind = 600;
    c.send();
    cromulence::heartbeat::PpsMsg m;
    for( size_t k=0;k<10;k++ )
    {
        testQ_.EXPECT_NO_MSG(100ms);
        m.send();
    }
    testQ_.EXPECT_MSG< moonlighter::io::AcbGetTlmMsg >(1s);
}

TEST_F( AttitudeAppTest , OnOffPointing  )
{
    cromulence::attitude::RequestMsg m;
    m.data.payload.qAxis[0] = 0.0;
    m.data.payload.qAxis[1] = 1.0;
    m.data.payload.qAxis[2] = 0.0;
    m.data.payload.qAxis[3] = 0.0;
    m.send();
    testQ_.EXPECT_NO_MSG(1s);
    cromulence::attitude::ConfigMsg c;
    c.data.payload.enabled = 1;
    c.data.payload.frame_count = 10;
    c.data.payload.max_blind = 600;
    c.send();
    m.send();
    auto o = testQ_.EXPECT_MSG< moonlighter::io::AcbPointMsg >(10s); 
    EXPECT_NEAR ( o.data.payload.qAxis[0] , 0.0 , 0.001 ); 
    EXPECT_NEAR ( o.data.payload.qAxis[1] , 1.0 , 0.001 ); 
    EXPECT_NEAR ( o.data.payload.qAxis[2] , 0.0 , 0.001 );
    EXPECT_NEAR ( o.data.payload.qScalar , 0.0 , 0.001 );
    c.data.payload.enabled = 0;
    c.send();
    m.send();
    testQ_.EXPECT_NO_MSG(1s);

}

TEST_F( AttitudeAppTest , Blinded  )
{
    cromulence::attitude::ConfigMsg c;
    c.data.payload.enabled = 1;
    c.data.payload.frame_count = 10;
    c.data.payload.max_blind = 600;
    c.send();
    moonlighter::io::AcbStateMsg s;
    s.data.payload.star_tacker_delay = 601;
    s.send();
    auto o = testQ_.EXPECT_MSG< moonlighter::io::AcbPointMsg >(10s); 
    cromulence::attitude::RequestMsg m;
    m.data.payload.qAxis[0] = 0.0;
    m.data.payload.qAxis[1] = 1.0;
    m.data.payload.qAxis[2] = 0.0;
    m.data.payload.qAxis[3] = 0.0;
    m.send();
    testQ_.EXPECT_NO_MSG(1s); 

}