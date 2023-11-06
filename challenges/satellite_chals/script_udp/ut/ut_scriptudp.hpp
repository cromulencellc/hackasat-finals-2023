#pragma once
#include "gtest/gtest.h"
#include <fstream>
#include <thread>
#include <filesystem>
#include "Geofence.hpp"
#include "subprocess.hpp"
#include "AppTester.hpp"
#include "IoMessages.hpp"
#include "GeofenceMessages.hpp"
#include "ScriptUdpMessages.hpp"
#include "ScriptMessages.hpp"

using namespace std::chrono_literals;
namespace SU = cromulence::script_udp;
namespace LS = cromulence::script;

namespace M = moonlighter::io;
namespace G = cromulence::geofence;
class ScriptUdpTest : public ::testing::Test
{
public:

protected:
    pid_t pid_;

    void SetUp()
    {
        testQ_.subscribe<M::NovatelCmdMsg>();
        testQ_.subscribe<G::DownloadRequestMsg>();
        testQ_.subscribe<G::SnapRequestMsg>();
        testQ_.subscribe<M::DevicePowerMsg>();
        testQ_.subscribe<SU::KiddieMsg>();
        std::this_thread::sleep_for( 10s );

    }
    void TearDown()
    {

    }
    ScriptUdpTest( ) : ::testing::Test(),
        testQ_( "ScriptUdpTest", 10 )
    {
    
    }
    ~ScriptUdpTest()
    {
    }
    cromulence::TestMsgQueue testQ_;

    void run_script( uint8_t id )
    {
        SU::TeamMsg t;

        LS::LoadMsg l;
        LS::SetUserMsg u;
        LS::RunMsg r;

        strncpy( u.data.payload.user , "user1" , LS::MAX_USER_SIZE);
        strncpy( t.data.payload.name , "user1" , LS::MAX_USER_SIZE);

        l.data.payload.script = id;
        r.data.payload.script = id;
        u.send();
        t.send();
        l.send();
        r.send();
    }
};

TEST_F( ScriptUdpTest, Script0_GPS )
{
    run_script( 0 );
    M::NovatelCmdMsg n;
    n = testQ_.EXPECT_MSG<M::NovatelCmdMsg>( 15s );
    
    EXPECT_STREQ( n.data.payload.command, "LOG BESTPOSA ONCE\r\n" );
}

TEST_F( ScriptUdpTest, Script1_SNAP )
{
    run_script( 1 );
    G::SnapRequestMsg s;
    
    s = testQ_.EXPECT_MSG<G::SnapRequestMsg>( 15s );
    EXPECT_EQ( s.data.payload.image_id , 0 );
}

TEST_F( ScriptUdpTest, Script2_GET )
{
    run_script( 2 );
    G::DownloadRequestMsg m;
    testQ_.EXPECT_MSG<G::DownloadRequestMsg>( 15s );
    EXPECT_EQ( m.data.payload.image_id , 0);
    
}
TEST_F( ScriptUdpTest, Script3_KIDDIE )
{
    run_script( 3 );
    SU::KiddieMsg ku;
    ku = testQ_.EXPECT_MSG<SU::KiddieMsg>(15s);
    EXPECT_STREQ( ku.data.payload.team , "user1");
}
TEST_F( ScriptUdpTest, Script4_CAM )
{
    run_script( 4 );
    M::DevicePowerMsg o;
    o = testQ_.EXPECT_MSG<M::DevicePowerMsg>(15s);
    EXPECT_EQ( o.data.payload.device , M::CAMERA_POWER );
    EXPECT_EQ( o.data.payload.power  , 1 );
}
TEST_F( ScriptUdpTest, Script5_IB )
{
    std::string flag_path{"./cf/download/user1.ib_flag"};
    
    // Remove previouse solve if it exists
    if(std::filesystem::exists(flag_path)){
        std::filesystem::remove(flag_path);
    }

    run_script( 5 );
    std::this_thread::sleep_for( 30s );
    EXPECT_TRUE( std::filesystem::exists(flag_path) );
    // Try Hard would be untaring the logs script, getting the key
    std::ifstream f( flag_path.c_str() );
    std::string username{};
    std::string secret_hash{};
    std::getline(f, username);
    std::getline(f, secret_hash);

    EXPECT_STREQ(username.c_str(), "user1");
}
TEST_F( ScriptUdpTest, Script6_Santa )
{
    run_script( 6 );

    char cmd_with_inject[] = "LOG BESTposa once\r\nuserdatum 6378206.0 294.97 0.0 0.0 0.0 0.0 0.0 3.14 0.0\r\nlog bestposA ONCE\r\n";
    
    M::NovatelCmdMsg n;
    n = testQ_.EXPECT_MSG<M::NovatelCmdMsg>( 15s );

    EXPECT_STREQ(n.data.payload.command, cmd_with_inject);
}