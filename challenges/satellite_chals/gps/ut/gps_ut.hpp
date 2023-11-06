#pragma once
#include "gtest/gtest.h"
#include "GpsMessages.hpp"
#include "IoMessages.hpp"
#include "AppTester.hpp"
#include <vector>
#include <iostream>

using namespace std::chrono_literals;
class GpsAppTest : public ::testing::Test
{


public:


protected:
    GpsAppTest( ) :
        testQ_( "atttest", 10 )
    {
    }
        void SetUp()
    {
        testQ_.subscribe<cromulence::gps::EcefMsg>( );
        testQ_.subscribe<cromulence::gps::GeodeticMsg>();
    }
    void TearDown()
    {
       
    }
    cromulence::TestMsgQueue testQ_;

};

TEST_F( GpsAppTest , NoOp )
{
    cromulence::gps::NoOpMsg m;
    m.send();
    testQ_.EXPECT_NO_MSG( 5s );

}


TEST_F( GpsAppTest , Reset )
{
    cromulence::gps::ResetMsg m;
    m.send();

    testQ_.EXPECT_NO_MSG( 5s );

}

TEST_F( GpsAppTest ,  BESTPOS1 )
{
    namespace M = moonlighter::io;
    namespace G = cromulence::gps;
    std::string response = "BESTPOSA,COM1,0,78.5,FINESTEERING,1419,336208.000,02000040,6145,2724;SOL_COMPUTED,NARROW_INT,51.11635910984,-114.03833105168,1063.8416,-16.2712,WGS84,0.0135,0.0084,0.0172,\"AAAA\",1.000,0.000,8,8,8,8,0,01,0,03";
    std::string ck = "*3d9fbd48";
    M::NovatelOutMsg m;
    m.data.payload.id = 0;
    strncpy( m.data.payload.response , response.c_str() , M::MAX_GPS_RESPONSE_SIZE );
    strncpy( m.data.payload.crc , ck.c_str() ,sizeof(m.data.payload.crc) );

    m.send();

    G::GeodeticMsg g;
    G::EcefMsg e;
    g = testQ_.EXPECT_MSG<G::GeodeticMsg>( 1s );
    e = testQ_.EXPECT_MSG<G::EcefMsg>( 1s );

    EXPECT_NEAR( g.data.payload.altitude , 1.063, 0.01 );
    EXPECT_NEAR( g.data.payload.latitude , 51.116,  0.01);
    EXPECT_NEAR( g.data.payload.longitude , -114.038, 0.01 );

    EXPECT_NEAR( e.data.payload.position[0] , -1634.536 , 1 );
    EXPECT_NEAR( e.data.payload.position[1] , -3664.627 , 1 );
    EXPECT_NEAR( e.data.payload.position[2] ,4942.509  , 1 );
}
TEST_F( GpsAppTest ,  Bad )
{
    namespace M = moonlighter::io;
    namespace G = cromulence::gps;
    std::string response = "BESTLOCA,COM1,0,78.5,FINESTEERING,1419,336208.000,02000040,6145,2724;SOL_COMPUTED,NARROW_INT,10.000000,90.0000,1000.00,-16.2712,WGS84,0.0135,0.0084,0.0172,\"AAAA\",1.000,0.000,8,8,8,8,0,01,0,03";
    std::string ck = "*3d9fbd48";
    M::NovatelOutMsg m;
    m.data.payload.id = 0;
    strncpy( m.data.payload.response , response.c_str() , M::MAX_GPS_RESPONSE_SIZE );
    strncpy( m.data.payload.crc , ck.c_str() ,sizeof(m.data.payload.crc) );

    m.send();

    testQ_.EXPECT_NO_MSG( 1s );
}

TEST_F( GpsAppTest , BadSoln )
{
       namespace M = moonlighter::io;
    namespace G = cromulence::gps;
    std::string response = "BESTPOSA,COM1,0,78.5,FINESTEERING,1419,336208.000,02000040,6145,2724;VARIANCE,NARROW_INT,10.000000,90.0000,1000.00,-16.2712,WGS84,0.0135,0.0084,0.0172,\"AAAA\",1.000,0.000,8,8,8,8,0,01,0,03";
    std::string ck = "*3d9fbd48";
    M::NovatelOutMsg m;
    m.data.payload.id = 0;
    strncpy( m.data.payload.response , response.c_str() , M::MAX_GPS_RESPONSE_SIZE );
    strncpy( m.data.payload.crc , ck.c_str() ,sizeof(m.data.payload.crc) );

    m.send();

    testQ_.EXPECT_NO_MSG( 1s ); 
}