#pragma once
#include "gtest/gtest.h"
#include "Geofence.hpp"
#include <cmath>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <iostream>
class GeofenceTest : public ::testing::Test
{
public:
    struct Truth
    {
        std::string time;
        Eigen::Quaterniond q;
        Eigen::Vector3d ecef;
    };
    std::vector<Truth> truth_;
    static constexpr double R_GEO = 42164 ;
    cromulence::Geofence g;

protected:
    GeofenceTest() 
        {
            
        }

    void SetUp()
    {   
        char truePath[256];
        OS_TranslatePath(  "/cf/restricted.json", truePath  );
        g.updateTimeOffsets( 37.0 , -0.0145 );
        g.from_json(truePath );
    }
    void TearDown()
    {
    
    }
    cromulence::Gregorian from_String( std::string timeStr) 
    {
        std::tm timeStruct;
        strptime( timeStr.c_str() , "%d %b %Y %H:%M:%S.%f", &timeStruct  );

        // Extract numeric values
        cromulence::Gregorian t;
        t.yr = timeStruct.tm_year + 1900;
        t.mo = timeStruct.tm_mon + 1;
        t.day = timeStruct.tm_mday;
        t.hr = timeStruct.tm_hour;
        t.min = timeStruct.tm_min;
        t.sec = static_cast<double>(timeStruct.tm_sec) ;//+ fractional_seconds.count();
        return t;
    }
    void readCSV( std::string filename )
    {
        std::fstream fin;
        std::string temp,line,word;
        // Open an existing file
        int cnt(0);
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
                switch(k)
                {
                case 0:
                    entry.time = std::string( word );
                    break;
                case 1:
                    entry.q.x() = std::atof( word.c_str( ) ) ;
                    break;
                case 2:
                    entry.q.y() = std::atof( word.c_str( ) ) ;
                    break;
                case 3:
                    entry.q.z() = std::atof( word.c_str( ) ) ;
                    break;
                case 4:
                    entry.q.w() = std::atof( word.c_str( ) ) ;
                    break;
                case 5:
                    entry.ecef.x() = std::atof( word.c_str( ) ) ;
                    break;
                case 6:
                    entry.ecef.y() = std::atof( word.c_str( ) ) ;
                    break;
                case 7:
                    entry.ecef.z() = std::atof( word.c_str( ) ) ;
                    break;
                default:
                    std::cout<<"BROKE"<<std::endl;
                    throw std::exception( );
                }

                k++;
            }
            truth_.push_back( entry );

        }
        std::cout<<"Sorted"<<std::endl;
    }

};


TEST_F( GeofenceTest ,  GeofenceEnforced )
{
    char truePath[256];
    OS_TranslatePath(  "/cf/sat_m100.csv", truePath  );
    readCSV( truePath);
    for( auto item: truth_ )
    {
        cromulence::Gregorian gt;
        gt = from_String( item.time );

        g.setTime( gt );
        double q[3] = { item.q.x(),  item.q.y() , item.q.z() };
        double qs = item.q.w();
        g.setLookQuaternion(qs ,q );
        double pos[3];
        pos[0] = item.ecef.x();
        pos[1] = item.ecef.y();
        pos[2] = item.ecef.z();
        g.setGpsPosition( pos );

        ASSERT_TRUE( g.calcRestricted() );
    }
}

TEST_F( GeofenceTest ,  GeofenceNotEnforced)
{
    char truePath[256];
    OS_TranslatePath(  "/cf/sat_m120.csv", truePath  );
    readCSV( truePath);
    for( auto item: truth_ )
    {
        cromulence::Gregorian gt;
        gt = from_String( item.time );

        g.setTime( gt );
        double q[3] = { item.q.x(),  item.q.y() , item.q.z() };
        double qs = item.q.w();
        g.setLookQuaternion(qs ,q );
        double pos[3];
        pos[0] = item.ecef.x();
        pos[1] = item.ecef.y();
        pos[2] = item.ecef.z();
        g.setGpsPosition( pos );

        ASSERT_FALSE( g.calcRestricted() );
    }
}

TEST_F( GeofenceTest ,  GeofenceCONUS)
{
    char truePath[256];
    OS_TranslatePath(  "/cf/sat_conus.csv", truePath  );
    readCSV( truePath);
    for( auto item: truth_ )
    {
        cromulence::Gregorian gt;
        gt = from_String( item.time );

        g.setTime( gt );
        double q[3] = { item.q.x(),  item.q.y() , item.q.z() };
        double qs = item.q.w();
        g.setLookQuaternion(qs ,q );
        double pos[3];
        pos[0] = item.ecef.x();
        pos[1] = item.ecef.y();
        pos[2] = item.ecef.z();
        g.setGpsPosition( pos );

        EXPECT_TRUE( g.calcRestricted() );
    }
}