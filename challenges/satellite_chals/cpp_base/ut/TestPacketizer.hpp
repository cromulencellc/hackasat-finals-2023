#include "gtest/gtest.h"

#include "Packetizer.hpp"
#include "PingMessages.hpp"
    namespace P = cromulence::ping;


class PacketizerTest : public ::testing::Test
{

protected:

    uint32_t S= 0xDEADBEEF;
    PacketizerTest() :
        p( S )
    {

    }
    ~PacketizerTest()
    {

    }


    static const size_t N = 10000; //buffer length
    static const size_t M = 1024; // max packet sz
    cromulence::Packetizer< N , M > p;
    
    void push_packet( void* pkt , size_t s )
    {
        p.push_bytes( &S ,sizeof(S));
        p.push_bytes( pkt ,s );
    }
};

TEST_F( PacketizerTest , NoSync)
{
    // make a message and push it 
    cromulence::ping::PingMsg m;

    p.push_bytes( m.as_cfs_ptr() , m.size());
    // 
    CFE_SB_Msg_t *out = p.get_packet();
    ASSERT_EQ( nullptr , out);


}
TEST_F( PacketizerTest , OneMessage)
{
    // make a message and push it 
    cromulence::ping::PingMsg m;

    push_packet( &m.data , m.size() );
    // 
    CFE_SB_Msg_t *out = p.get_packet();
    ASSERT_NE( nullptr , out);

    EXPECT_NO_THROW( cromulence::ping::PingMsg m1( out ) ) ;
    // i should get the same packet if i try again (because i didnt use pop)
    out = p.get_packet();
    ASSERT_NE( nullptr , out);

}

TEST_F( PacketizerTest , PacketDestroyed )
{
    // make a message and push it 
    cromulence::ping::PingMsg m;
    push_packet( &m.data , m.size() );
    // 
    CFE_SB_Msg_t *out = p.get_packet();
    ASSERT_NE( nullptr , out);

    EXPECT_NO_THROW( cromulence::ping::PingMsg m1( out ) ) ;
    p.pop_packet();
    out = p.get_packet();
    EXPECT_EQ( nullptr , out);
}
TEST_F( PacketizerTest , ParitalHeader)
{

    cromulence::ping::PingMsg m;
    // push al but the last header byte
    push_packet( &m.data , sizeof( CFE_SB_Msg_t )-1 );
    // 
    CFE_SB_Msg_t *out = p.get_packet();
    EXPECT_EQ( nullptr , out );
}

TEST_F( PacketizerTest , PartialData)
{
    cromulence::ping::PingMsg m;
    // push al but the last header byte
    push_packet( &m.data , m.size() - 1 );
    // 
    CFE_SB_Msg_t *out = p.get_packet();
    EXPECT_EQ( nullptr , out );
}
TEST_F( PacketizerTest , TwoMessages)
{
    cromulence::ping::PingMsg m;
    push_packet( &m.data , m.size() );
    cromulence::ping::ResetMsg r;
    push_packet( &r.data , r.size() );

    CFE_SB_Msg_t *out = p.get_packet();
    ASSERT_NE( nullptr, out );
    EXPECT_NO_THROW( cromulence::ping::PingMsg m1( out ) ) ;
    p.pop_packet();
    out = p.get_packet( );
    ASSERT_NE( nullptr, out );
    EXPECT_NO_THROW( cromulence::ping::ResetMsg m2( out ) ) ;
    p.pop_packet();
    out = p.get_packet( );
    EXPECT_EQ( nullptr, out );
}
TEST_F( PacketizerTest, SyncInHeader )
{
    //  | Sync word | Header with 0xFFFF length | sync word |  
    uint16_t bad;
    CFE_SB_Msg_t *out;
    bad = 0xFFFF;
    p.push_bytes( &S ,sizeof(S));
    p.push_bytes( &bad , sizeof(bad));
    
    out = p.get_packet();
    EXPECT_EQ( nullptr  , out );
    // Push two packets
    cromulence::ping::PingMsg m;
    strncpy( m.data.payload.data , "Lolcats", P::PING_SIZE );
    m.add_checksum();
    push_packet( &m.data , m.size() );
    strncpy( m.data.payload.data , "Nullcats", P::PING_SIZE );
    m.add_checksum();
    push_packet( &m.data , m.size() );
        
    // Get da first message
    cromulence::ping::PingMsg ping;
    out = p.get_packet( );
    ASSERT_NE( out , nullptr );
    bool ok = CFE_SB_ValidateChecksum( out );
    EXPECT_TRUE( ok );
    EXPECT_NO_THROW( ping = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "Lolcats" , ping.data.payload.data );
    p.pop_packet();
    // Get da second message
    out = p.get_packet( );
    ASSERT_NE( out , nullptr );
    ok = CFE_SB_ValidateChecksum( out );
    EXPECT_TRUE( ok );
    EXPECT_NO_THROW( ping = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "Nullcats" , ping.data.payload.data );
}
TEST_F( PacketizerTest, BadCaseDesync)
{
    //  | Sync word | Header with 0xFFFFFFFFFFFFFFFF length | sync word |  
    uint64_t bad;
    CFE_SB_Msg_t *out;
    bad = 0xFFFFFFFFFFFFFFFF;
    p.push_bytes( &S ,sizeof(S));
    p.push_bytes( &bad , sizeof(bad));
    
    out = p.get_packet();
    EXPECT_EQ( nullptr  , out );
    // Push two packets
    cromulence::ping::PingMsg m;
    strncpy( m.data.payload.data , "Lolcats", P::PING_SIZE );
    m.add_checksum();
    push_packet( &m.data , m.size() );
    strncpy( m.data.payload.data , "Nullcats", P::PING_SIZE );
    m.add_checksum();
    push_packet( &m.data , m.size() );
        
    // Get da first message
    cromulence::ping::PingMsg ping;
    out = p.get_packet( );
    ASSERT_NE( out , nullptr );
    bool ok = CFE_SB_ValidateChecksum( out );
    EXPECT_TRUE( ok );
    EXPECT_NO_THROW( ping = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "Lolcats" , ping.data.payload.data );
    p.pop_packet();
    // Get da second message
    out = p.get_packet( );
    ASSERT_NE( out , nullptr );
    ok = CFE_SB_ValidateChecksum( out );
    EXPECT_TRUE( ok );
    EXPECT_NO_THROW( ping = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "Nullcats" , ping.data.payload.data );
    

}


TEST_F( PacketizerTest , DesynchOnPacketCutoff )
{
    // fill a whole packet into the buffer
    cromulence::ping::PingMsg m;
    cromulence::ping::PingMsg o;

    strncpy( m.data.payload.data , "LolCats", P::PING_SIZE);
    push_packet( &m.data , m.size() ); // Packet 1
    // now fill with first half a packet including sync - like if we got cut off part way through a transmission
    push_packet( &m.data , m.size()-1 ); // Packet 2
    // REmove one packet so we are left with a partial packet
    CFE_SB_Msg_t *out = p.get_packet();
    EXPECT_NO_THROW( o = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "LolCats" , o.data.payload.data );
    p.pop_packet();
    // TEST: no new packets
    out = p.get_packet( );
    EXPECT_EQ( out , nullptr );
    // now put in 2 full packet - like we started a new transmission
    strncpy( m.data.payload.data , "OverflowCats", P::PING_SIZE);
    m.add_checksum();

    push_packet( &m.data , m.size() ); // Packet 3
    strncpy( m.data.payload.data , "NullCats", P::PING_SIZE);
    m.add_checksum();

    push_packet( &m.data , m.size() ); // Packet 4
    // Packet 2 should be detected as a double sync and be dropped
    // Packet 3 shuld be next 
    out = p.get_packet( );
    ASSERT_NE( out , nullptr );
    bool ok = CFE_SB_ValidateChecksum( out );
    EXPECT_TRUE( ok );
    EXPECT_NO_THROW( o = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "OverflowCats" , o.data.payload.data );
    p.pop_packet(); 
    // Now we should get packet 3
    out = p.get_packet( );
    ASSERT_NE( out , nullptr );
    EXPECT_NO_THROW( o = cromulence::ping::PingMsg( out ));
    EXPECT_STREQ( "NullCats" , o.data.payload.data );
    
    p.pop_packet();    
    // TEST: no new packets (buffer empty)
    out = p.get_packet( );
    EXPECT_EQ( out , nullptr );

}

TEST_F( PacketizerTest , DesynchOnPartialBegin )
{

    // fill with the back half of a packet - like if we started a new contact but only had partial transmission
    cromulence::ping::ResetMsg r;
    uint8_t *backhalf = reinterpret_cast<uint8_t*>(&r.data) + r.size()/2;
    p.push_bytes( backhalf , r.size()/2 );
    // TEST: no new packets 
    CFE_SB_Msg_t *out = p.get_packet();
    EXPECT_EQ( out , nullptr );
    // now add a full packet - like we get to keep going 
    cromulence::ping::PingMsg m;
    push_packet( &m.data , m.size() );
    
    // TEST: Remove one packet
    out = p.get_packet();
    ASSERT_NE( out , nullptr );
    int c = std::memcmp( &m.data , out ,m.size() );
    EXPECT_EQ(c, 0 );
    p.pop_packet();
    // TEST: no new packets (buffer empty)
    out = p.get_packet( );
    EXPECT_EQ( out , nullptr );

}