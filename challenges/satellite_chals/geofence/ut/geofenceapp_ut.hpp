#include "gtest/gtest.h"
#include <thread>
#include <chrono>
#include "GeofenceMessages.hpp"
#include "GpsMessages.hpp"
#include "IoMessages.hpp"
#include "AppTester.hpp"
#include <filesystem>
using namespace std::chrono_literals;
class GeofenceAppTest : public ::testing::Test
{
public:


protected:
    GeofenceAppTest( ) :
        testQ_( "geofencetest", 10 )
    {
    }
        void SetUp()
    {
        namespace G = cromulence::geofence;
        testQ_.subscribe<G::GeofenceZoneMsg>( );
        testQ_.subscribe<G::RejectedMsg>( );
        testQ_.subscribe<moonlighter::io::ImageSnapMsg>();

        cromulence::geofence::TeamChangeMsg t;
        strcpy( t.data.payload.name , "user1");
        t.send();
        moonlighter::io::PollingConfigMsg p;


        p.data.payload.allowed = 1;
        p.send( );
    }
    void TearDown()
    {
        std::this_thread::sleep_for(5s);
       
    }
    void send_good_gps()
    {
    cromulence::gps::EcefMsg ecef;
    ecef.data.payload.position[0] = -21083.129334;
    ecef.data.payload.position[1] = -36517.051189;
    ecef.data.payload.position[2] = 0;
    ecef.send();
    }
    void send_good_quat()
    {
    moonlighter::io::AcbStateMsg state;
    state.data.payload.qAxis[0] = -0.500643;
    state.data.payload.qAxis[1] = -0.498227;
    state.data.payload.qAxis[2] = 0.499371;
    state.data.payload.qScalar  = 0.501752;
    state.data.payload.utc.yr  = 23;
    state.data.payload.utc.min = 5;
    state.data.payload.utc.day = 22;
    state.data.payload.utc.hr = 16;
    state.data.payload.utc.min = 0;
    state.data.payload.utc.sec = 0;
    state.data.payload.utc.csec=0;
    state.send();
    }
    cromulence::TestMsgQueue testQ_;

};

using namespace std::chrono_literals;
TEST_F( GeofenceAppTest , NoOp  )
{

    cromulence::geofence::NoOpMsg nop;
    nop.send();
    testQ_.EXPECT_NO_MSG(1s);
}
TEST_F( GeofenceAppTest , Reset  )
{

    cromulence::geofence::ResetMsg reset;
    reset.send();
    testQ_.EXPECT_NO_MSG(1s);
}
TEST_F( GeofenceAppTest , GetFences  )
{

    cromulence::geofence::ResetMsg reset;
    reset.send();
    testQ_.EXPECT_NO_MSG(1s);
}

TEST_F( GeofenceAppTest , SpamRequest )
{
    cromulence::geofence::DownloadRequestMsg r;
    r.send();
    r.send();
    auto x = testQ_.EXPECT_MSG<moonlighter::io::ImageGetMsg>(1s);
    testQ_.EXPECT_NO_MSG(1s);
}
TEST_F( GeofenceAppTest , SpamSnap )
{
    send_good_gps();
    send_good_quat();

    cromulence::geofence::SnapRequestMsg r;
    r.send();
    r.send();
    auto x = testQ_.EXPECT_MSG<moonlighter::io::ImageSnapMsg>(1s);
    testQ_.EXPECT_NO_MSG(1s);
}
TEST_F( GeofenceAppTest , BadSnap  )
{
    //
    cromulence::geofence::TimeOffsetsMsg t;
    t.data.payload.deltaUT1 = -0.0145;
    t.data.payload.leapseconds = 37.0;
    t.send();
    // Pick something that is on the xaxis
    cromulence::gps::EcefMsg ecef;
    ecef.data.payload.position[0] = -7322.093977;
    ecef.data.payload.position[1] = -41525.658451;
    ecef.data.payload.position[2] = 0;
    ecef.send();
    //
    moonlighter::io::AcbStateMsg state;
    state.data.payload.qAxis[0] = -0.406323;
    state.data.payload.qAxis[1] = -0.577786;
    state.data.payload.qAxis[2] = 0.578720;
    state.data.payload.qScalar  = 0.407614;
    state.data.payload.utc.yr  = 123;
    state.data.payload.utc.mo = 5;
    state.data.payload.utc.day = 22;
    state.data.payload.utc.hr = 16;
    state.data.payload.utc.min = 0;
    state.data.payload.utc.sec = 0;
    state.data.payload.utc.csec=0;
    state.send();
    // now snappy snap
    cromulence::geofence::SnapRequestMsg req;
    req.data.payload.image_id = 1;
    req.send();
    cromulence::geofence::RejectedMsg o;
    o = testQ_.EXPECT_MSG<cromulence::geofence::RejectedMsg>( 1s );
    EXPECT_STREQ( "not_here" , o.data.payload.name );
    std::this_thread::sleep_for(1s);
}
TEST_F( GeofenceAppTest , GoodSnapQTimeout  )
{

    // Pick something that is on the xaxis
    send_good_quat();
    std::this_thread::sleep_for( 20s );
    send_good_gps();
    cromulence::geofence::SnapRequestMsg req;
    req.data.payload.image_id = 0;
    req.send();
    moonlighter::io::ImageSnapMsg o;
    testQ_.EXPECT_NO_MSG(1s);

    //EXPECT_EQ( o.data.payload.count , 2 );
    //EXPECT_EQ( o.data.payload.delay , 10 );
}
TEST_F( GeofenceAppTest , GoodSnapPosTimeout  )
{

    // Pick something that is on the xaxis
    send_good_gps();
    std::this_thread::sleep_for( 20s );

    send_good_quat();

    cromulence::geofence::SnapRequestMsg req;
    req.data.payload.image_id = 0;
    req.send();
    moonlighter::io::ImageSnapMsg o;
    testQ_.EXPECT_NO_MSG(1s);
    //EXPECT_EQ( o.data.payload.count , 2 );
    //EXPECT_EQ( o.data.payload.delay , 10 );
}
TEST_F( GeofenceAppTest , GoodSnap  )
{

    // Pick something that is on the xaxis
    send_good_gps();
    send_good_quat();

    cromulence::geofence::SnapRequestMsg req;
    req.data.payload.image_id = 0;
    req.send();
    moonlighter::io::ImageSnapMsg o;
    o = testQ_.EXPECT_MSG<moonlighter::io::ImageSnapMsg>( 1s );
    std::this_thread::sleep_for( 1s) ;
    //EXPECT_EQ( o.data.payload.count , 2 );
    //EXPECT_EQ( o.data.payload.delay , 10 );
}

TEST_F( GeofenceAppTest , Download )
{

    cromulence::geofence::DownloadRequestMsg r;
    r.data.payload.image_id = 0;
    r.send();
    moonlighter::io::ImageGetMsg o;
    o = testQ_.EXPECT_MSG<moonlighter::io::ImageGetMsg>( 1s );
    EXPECT_STREQ( "user1_0.jpg", o.data.payload.img);
    EXPECT_STREQ( "./images/" , o.data.payload.download_directory );
} 

TEST_F( GeofenceAppTest , BlockedDownload )
{
    moonlighter::io::PollingConfigMsg p;

    cromulence::geofence::DownloadRequestMsg r;
    r.data.payload.image_id = 0;

    p.data.payload.allowed = 0;
    p.send();
    r.send();
    testQ_.EXPECT_NO_MSG( 1s );
    p.data.payload.allowed = 1;
    p.send();
    r.send();
    moonlighter::io::ImageGetMsg o;
    o = testQ_.EXPECT_MSG<moonlighter::io::ImageGetMsg>( 1s );
    EXPECT_STREQ( "user1_0.jpg", o.data.payload.img);
    EXPECT_STREQ( "./images/" , o.data.payload.download_directory );
} 