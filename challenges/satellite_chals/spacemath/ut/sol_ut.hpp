#pragma once
#include "gtest/gtest.h"
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include "TimeScale.hpp"
#include "Sol.hpp"
#include <eigen3/Eigen/Dense>
extern "C" {
   #include "cfe.h"
}

class SolTest : public ::testing::Test
{
public:
    struct Truth
    {
        std::string time;
        Eigen::Vector3d j2000;
        Eigen::Vector3d ecef;
        Eigen::Vector3d sol;
    };
    std::vector< Truth > truth_;
    cromulence::TimeScale ts_;
    cromulence::Sol sol_;
    static constexpr double POS_TOLERANCE = 10000.0;
    
protected:
    SolTest() 
    {
        // NIST gives us these values for times later than 
        // 2023-02-24
        // dUT1 =-14.5 ms 
        // dAt = 37.0
            ts_.setOffsets(-0.0145,37.0);
    }

    void SetUp()
    {

    }
    void TearDown()
    {
    
    }
    void readCSV( std::string filename )
    {
        std::fstream fin;
        std::string temp,line,word;
        // Open an existing file
        fin.open( filename , std::ios::in);
        double row[8];
        while (!fin.eof()) {
            // read an entire row and
            // store it in a string variable 'line'
            getline(fin, line);
  
            // used for breaking words
            std::stringstream s(line);

            //
            Truth entry;
            // read every column data of a row and
            // store it in a string variable, 'word'
            size_t k=0;
            while (getline(s, word, ',')) 
            {
                if( k ==0 )
                {
                    entry.time = std::string( word );
                }  
                else if( k>0 && k < 4 )
                {
                    // entries 1,2,3
                    entry.j2000[ (k-1) ] =  std::atof(word.c_str());
                }
                else if ( k >=4  && k <=6 )
                {
                    // entries 4,5,6
                    entry.ecef[k-4] = std::atof(word.c_str());
                }
                else
                {
                    // 7,8,9
                    entry.sol[k-7] = std::atof( word.c_str( ));
                }
                k++;
            }
            truth_.push_back( entry );
        }
    }
};


TEST_F( SolTest ,  GMatComparison )
{
    char truePath[256];
    OS_TranslatePath(  "/cf/frame_truth.csv", truePath  );
    readCSV( truePath );
    int cnt= 0 ;
    for( auto item : truth_ )
    {
    
        std::tm timeStruct = {};
        std::istringstream iss(item.time );
        iss >> std::get_time(&timeStruct, "%d %b %Y %H:%M:%S");

        // Check if the input was successfully parsed
        if (iss.fail()) {
            std::cout << "Failed to parse the input string." << std::endl;
            break;
        }

        // Convert std::tm to std::chrono::system_clock::time_point
        std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::from_time_t(std::mktime(&timeStruct));

        // Extract individual components from the time_point
        //std::chrono::year_month_day ymd;//= std::chrono::year_month_day(timePoint);
        auto tp = std::chrono::time_point_cast<std::chrono::days>(timePoint);

        auto ymd = std::chrono::year_month_day{tp};
        auto hms = std::chrono::time_point_cast<std::chrono::seconds>(timePoint) - tp;
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(timePoint);

        std::chrono::duration<double> fractional_seconds = timePoint - seconds;

       // std::chrono::hh_mm_ss hms = std::chrono::hh_mm_ss(timePoint);

        // Extract numeric values
        cromulence::Gregorian t;
        std::time_t tt = std::chrono::system_clock::to_time_t(timePoint);
        std::tm local_tm = *std::localtime(&tt);
        t.yr = local_tm.tm_year + 1900;
        t.mo = local_tm.tm_mon + 1;
        t.day = local_tm.tm_mday;
        t.hr = local_tm.tm_hour;
        t.min = local_tm.tm_min;
        t.sec = static_cast<double>(local_tm.tm_sec) + fractional_seconds.count();
    
        cromulence::AstronomicalTime astro;
        astro = ts_.from_utc( t );
        sol_.setTime( astro );
        Eigen::Vector3d posEci;

        posEci = sol_.eci_position();
        EXPECT_NEAR( posEci[0], item.sol[0], POS_TOLERANCE);
        EXPECT_NEAR( posEci[1], item.sol[1], POS_TOLERANCE);
        EXPECT_NEAR( posEci[2], item.sol[2], POS_TOLERANCE);
        cnt++;
    }
    std::cout<<"Verified :"<< cnt << " positions"<< std::endl;

}