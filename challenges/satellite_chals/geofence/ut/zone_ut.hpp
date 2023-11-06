#pragma once
#include "gtest/gtest.h"
#include "Zone.hpp"
#include <cmath>
class ZoneTest : public ::testing::Test
{
public:
    
    
protected:
    ZoneTest() 
        {
            
        }

    void SetUp()
    {

    }
    void TearDown()
    {
    
    }


};

bool NSidedTest( size_t N , double R , double TOL )
{    cromulence::Zone zone;
    double dA( (2.0*M_PI)/ N );
    for( size_t k= 0 ; k < N ; k++)
    {
        zone.append_point( cromulence::Point( R*cos(k*dA) , R*sin(k*dA) ) );
    }
    double RI = R - TOL; // radius just inside a vertex
    double RO = R + TOL; // radius just outside a vertex
    double RM = ( R* cos(dA / 2.0 ));
    double ROM = RM + TOL ; // radius just outside a midpoint
    double RIM = RM - TOL; // radius just inside a midpoint
    for( size_t k = 0; k < N ;k++)
    {
        double vertexAngle = k*dA;
        double midPointAngle = (k*dA) + dA/2.0;
        // check just inside the vertex
        EXPECT_TRUE( zone.isInside( cromulence::Point( RI*cos(vertexAngle), RI*sin(vertexAngle))) );
        // check just outside the vertex
        EXPECT_FALSE( zone.isInside( cromulence::Point( RO*cos(vertexAngle), RO*sin(vertexAngle))) );
        // check just insidde the midpoint
        EXPECT_TRUE( zone.isInside( cromulence::Point( RIM*cos(midPointAngle), RIM*sin(midPointAngle))));
        // check just outside the midpoint
        EXPECT_FALSE( zone.isInside( cromulence::Point( ROM*cos(midPointAngle), ROM*sin(midPointAngle))));
    }
}

using namespace std::chrono_literals;
// Do some detailed testing for square and trapezoid
TEST_F( ZoneTest , Square )
{
    cromulence::Zone zone;
    zone.append_point( cromulence::Point( 10.0 , 10.0 ) );
    zone.append_point( cromulence::Point( 10.0 , 20.0 ) );
    zone.append_point( cromulence::Point( 20.0 , 20.0 ) );
    zone.append_point( cromulence::Point( 20.0 , 10.0 ) );
    // Pick a bunch of good interior points
    EXPECT_TRUE( zone.isInside( cromulence::Point( 15.0,15.0))); //center
    EXPECT_TRUE( zone.isInside( cromulence::Point( 10.1,10.1))); //lower left
    EXPECT_TRUE( zone.isInside( cromulence::Point( 10.1,19.9))); //upper left
    EXPECT_TRUE( zone.isInside( cromulence::Point( 19.9,19.9))); //upper right
    EXPECT_TRUE( zone.isInside( cromulence::Point( 19.9,10.1))); //lower right
    // Pick a bunch of known outside points
    EXPECT_FALSE( zone.isInside( cromulence::Point( 0.0,0.0))); // origin is way outside
    EXPECT_FALSE( zone.isInside( cromulence::Point( 15.0,20.1))); // above
    EXPECT_FALSE( zone.isInside( cromulence::Point( 15.0,9.9))); // below
    EXPECT_FALSE( zone.isInside( cromulence::Point( 9.9,15.0))); // left
    EXPECT_FALSE( zone.isInside( cromulence::Point( 20.1,15.0))); // right

}

TEST_F( ZoneTest , Trapezoid )
{
    cromulence::Zone zone;
    zone.append_point( cromulence::Point( 10.0 , 10.0 ) );
    zone.append_point( cromulence::Point( 12.0 , 20.0 ) );
    zone.append_point( cromulence::Point( 18.0 , 20.0 ) );
    zone.append_point( cromulence::Point( 20.0 , 10.0 ) );
    // Pick a bunch of good interior points
    EXPECT_TRUE( zone.isInside( cromulence::Point( 15.0,15.0))); //interior
    EXPECT_TRUE( zone.isInside( cromulence::Point( 10.1,10.1))); //lower left
    EXPECT_TRUE( zone.isInside( cromulence::Point( 12.1,19.9))); //upper left
    EXPECT_TRUE( zone.isInside( cromulence::Point( 17.9,19.9))); //upper right
    EXPECT_TRUE( zone.isInside( cromulence::Point( 19.9,10.1))); //lower right
    EXPECT_TRUE( zone.isInside( cromulence::Point( 11.1,15.0))); // left-center
    EXPECT_TRUE( zone.isInside( cromulence::Point( 18.9,15.0))); // right-center
    
    // Pick a bunch of known outside points
    EXPECT_FALSE( zone.isInside( cromulence::Point( 0.0,0.0))); // origin is way outside
    EXPECT_FALSE( zone.isInside( cromulence::Point( 15.0,20.1))); // above
    EXPECT_FALSE( zone.isInside( cromulence::Point( 15.0,9.9))); // below
    EXPECT_FALSE( zone.isInside( cromulence::Point( 10.9,15.0))); // left-center
    EXPECT_FALSE( zone.isInside( cromulence::Point( 19.1,15.0))); // right-center


}
// generalized testing for a few other regular shapes.
TEST_F(ZoneTest, Triangle )
{
    NSidedTest( 3 , 10 , 0.1 );
}
TEST_F(ZoneTest, Pentagon )
{
    NSidedTest( 5 , 10 , 0.1 );
}

TEST_F(ZoneTest, Hexagon )
{
    NSidedTest( 7 , 10 , 0.1 );
}

TEST_F(ZoneTest, Octagon )
{
    NSidedTest( 8 , 10 , 0.1 );
}