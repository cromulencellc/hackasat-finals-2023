#pragma once
#include "gtest/gtest.h"
#include <cmath>
#include "TimeScale.hpp"

class TimeTest : public ::testing::Test
{
public:

    
protected:
    TimeTest() 
    {
        // NIST gives us these values for times later than 
        // 2023-02-24
        // dUT1 =-14.5 ms 
        // dAt = 37.0
    }

    void SetUp()
    {

    }
    void TearDown()
    {
    
    }

    static constexpr double TIME_TOL = 0.0001;
};

TEST_F( TimeTest , ValladoComparison )
{
    // Vallado gives the following results
    //   [ut1, tut1, jdut1, jdut1frac, utc, tai, tt, ttt, jdtt, jdttfrac, tdb, ttdb, jdtdb, jdtdbfrac ] = convtime ( 2000, 01,01, 11, 59, 28, 0, 0.014 , 37)
    //   ut1 =        43168.014
    //   tut1 = -1.013574740561943e-08
    //   jdut1 =        2451544.5
    //   jdut1frac = 0.4996297916666667
    //   utc =            43168
    //   tai =            43205
    //   tt =        43237.184
    //   ttt = 1.17828943279e-08
    //   jdtt =        2451544.5
    //   jdttfrac = 0.5004303703703704
    //   tdb = 43237.18390425495
    //   ttdb = 1.178286882967756e-08
    //   jdtdb =        2451544.5
    //   jdtdbfrac = 0.5004303692622101
    cromulence::TimeScale ts;
    ts.setOffsets( 0.014, 37.0 );
    cromulence::Gregorian t_greg;
    t_greg.yr = 2000;
    t_greg.mo = 01;
    t_greg.day = 01;
    t_greg.hr  = 11;
    t_greg.min = 59;
    t_greg.sec = 28;
    cromulence::AstronomicalTime astro = ts.from_utc( t_greg );

    EXPECT_NEAR( astro.ut1 ,  43168.014 ,TIME_TOL);
    EXPECT_NEAR( astro.jd_ut1.days ,  2451544.5 ,TIME_TOL);
    EXPECT_NEAR( astro.jd_ut1.frac ,  0.4996297916666667 ,TIME_TOL);
    EXPECT_NEAR( astro.tt, 43237.184, TIME_TOL);
    EXPECT_NEAR( astro.ttt ,  1.17828943279e-08 ,TIME_TOL);
    EXPECT_NEAR( astro.jd_tt.days ,2451544.5 ,  TIME_TOL);
    EXPECT_NEAR( astro.jd_tt.frac , 0.5004303703703704, TIME_TOL);
    EXPECT_NEAR( astro.tdb,  43237.18390425495, TIME_TOL);
    EXPECT_NEAR( astro.ttdb , 1.178286882967756e-08, TIME_TOL);
    EXPECT_NEAR( astro.jd_tdb.days ,2451544.5 ,  TIME_TOL);
    EXPECT_NEAR( astro.jd_tdb.frac ,0.5004303692622101 , TIME_TOL);
    
}