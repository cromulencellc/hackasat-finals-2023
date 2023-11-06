#include "gtest/gtest.h"

#include "PacketFilter.hpp"
#include "PingMessages.hpp"

#include "PingMessages.hpp"

class PacketFilterTest : public ::testing::Test
{


protected:
    PacketFilterTest() 
    {

    }
    ~PacketFilterTest()
    {

    }


    cromulence::packets::Filter filter_;
    static const uint16_t MAX_APID = 2048;
};


TEST_F( PacketFilterTest , Allowed )
{
    static const uint16_t TEST_APID = 0x1810;
    cromulence::CmdMessage< uint64_t, TEST_APID , 0> msg;
    filter_.modify_filter(  TEST_APID , true );
    EXPECT_TRUE(  filter_.allowed( msg.as_cfs_ptr() ) );
}
TEST_F( PacketFilterTest , NotAllowed )
{
    static const uint16_t TEST_APID = 0x1810;
    cromulence::CmdMessage< uint64_t, TEST_APID+1 , 0> msg;
    filter_.modify_filter(  TEST_APID , true );
    EXPECT_FALSE(  filter_.allowed( msg.as_cfs_ptr() ) );
}

TEST_F( PacketFilterTest , BadCheckSum )
{
    static const uint16_t TEST_APID = 0x1810;
    cromulence::CmdMessage< uint64_t, TEST_APID , 0> msg;
    msg.data.payload = 0xfeedbeefcafeface;// lets put some bits into the message
    CFE_SB_Msg_t* m = const_cast< CFE_SB_Msg_t* >( msg.as_cfs_ptr() );
    CFE_SB_GenerateChecksum( m );
    bool ok;
    ok = filter_.valid_checksum( m );
    EXPECT_TRUE( ok );
    // lets change the data now the checksum should be mismatch'd
    msg.data.payload = 0x1122334455667788;

    ok = filter_.valid_checksum( m );
    EXPECT_FALSE( ok );
}

TEST_F( PacketFilterTest , CommandIdentification )
{
    static const uint16_t TEST_APID = 0x1810;
    static const uint16_t TEST_APID2 = 0x0810;
    cromulence::CmdMessage< uint64_t, TEST_APID , 0> cmd;
    cromulence::TlmMessage< uint64_t, TEST_APID2 > tlm;

    EXPECT_FALSE( filter_.is_tlm( cmd.as_cfs_ptr() ));
    EXPECT_TRUE( filter_.is_cmd( cmd.as_cfs_ptr() ));
    EXPECT_TRUE( filter_.is_tlm( tlm.as_cfs_ptr() ));
    EXPECT_FALSE( filter_.is_cmd( tlm.as_cfs_ptr() ));
}

TEST_F( PacketFilterTest , Decimation )
{
    static const uint16_t TEST_APID = 0x1810;
    static const size_t TEST_DECIM = 5; 
    static const size_t EXPECTED = 20;
    cromulence::CmdMessage< uint64_t, TEST_APID , 0> cmd;

    filter_.modify_decimation( TEST_APID , 5 );
    size_t count(0);
    for( size_t k=0; k < TEST_DECIM*EXPECTED; k++ )
    {
        count += filter_.decimated( cmd.as_cfs_ptr() ) ? 1 : 0;
    }
    EXPECT_EQ( count , EXPECTED );
}
TEST_F( PacketFilterTest , Routing )
{
    static const size_t TEST_APID = 0x1805; 
    static const size_t TEST_ROUTE = 20;
    filter_.modify_route( TEST_APID , TEST_ROUTE );
    cromulence::CmdMessage< uint64_t, TEST_APID , 0> cmd1;
    cromulence::CmdMessage< uint64_t, TEST_APID+1 , 0> cmd2;
    
    EXPECT_EQ( TEST_ROUTE , filter_.routing( cmd1.as_cfs_ptr() ));
    EXPECT_EQ( 0 , filter_.routing( cmd2.as_cfs_ptr() ));//default
}
