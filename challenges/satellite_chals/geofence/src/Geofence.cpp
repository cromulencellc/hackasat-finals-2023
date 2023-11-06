#include "Geofence.hpp"
#include <cmath>
#include <nlohmann/json.hpp>
#include <fstream>
#include "TimeScale.hpp"
#include "Earth.hpp"
#include <iostream>
#include <filesystem>

using json = nlohmann::json;
namespace cromulence
{
    Geofence::Geofence() :
    timeEpoch_(0.0),
    rejected_("")
    {

    }

    Geofence::~Geofence()
    {

    }
    Eigen::Vector3d Geofence::getGps()
    {
        return posEcef_;
    }
    
    std::string Geofence::download_path( )
    {
        return downloadPath_;
    }
    void Geofence::updateTimeOffsets( double deltaAT, double deltaUT )
    {
        ts_.setOffsets( deltaUT , deltaAT );
    }
    void Geofence::from_json( std::string filename )
    {
        
        std::string err;
        std::ifstream f( filename.c_str() );
        json data =  json::parse(f);
        downloadPath_ = data["download_path"];
        if( false == std::filesystem::exists( downloadPath_ ) )
        {
            std::filesystem::create_directories( downloadPath_ );
        }
        auto zoneList = data["zones"];
        for( auto& [key, pointList]  : zoneList.items() )
        {
            std::string name(key);
            Zone newZone(name);

            for( auto item: pointList )
            {
                Point next;
                next.h = item["long"];
                next.v = item["lat"];
                newZone.append_point( next );
            }
            restricted_.push_back( newZone );

        }
        
    }
    void Geofence::setLookQuaternion( double qs , double qv[3] )
    {
        Eigen::Quaterniond q;
        q.w() = qs;
        q.x() = qv[0];
        q.y() = qv[1];
        q.z() = qv[2];
        q.normalize();
        // This rotation matrix is ROW MAJOR - so transpose it
        D_i_b_ = q.toRotationMatrix().transpose();
    }

    void Geofence::setGpsPosition(  double pos[3])
    {
       
        posEcef_.x() = pos[0];
        posEcef_.y() = pos[1];
        posEcef_.z() = pos[2];
    }
    void Geofence::setTime( Gregorian utc )
    {
        astroTime_= ts_.from_utc( utc );
    }
    double Geofence::getTAI( )
    {
        return astroTime_.tai;
    }
    double Geofence::getJdUtc() 
    {
        return astroTime_.jd_utc.days + astroTime_.jd_utc.frac;
    }
    bool Geofence::calcRestricted( )
    {
        
        rejected_ = "";
        // Calculate the unit vector for line of sight of the zaxis (camera is on z )
        Eigen::Matrix3d D_b_i = D_i_b_.transpose();
        // TODO these need real values yike!
        Eigen::Matrix3d D_i_e = earth_.j2000_to_fixed( astroTime_ );
        //std::cout<<"Body to J2000: "<<D_b_i<<std::endl;
        //std::cout<<"J2000 to ECEF"<<D_i_e<<std::endl;
        Eigen::Vector3d lookAxisBody;
        lookAxisBody << 0,0,1;
        Eigen::Vector3d lookAxisJ2000;
        lookAxisJ2000 = D_b_i * lookAxisBody;
        //std::cout<<"Look J2000: "<<lookAxisJ2000<<std::endl;
        
        Eigen::Vector3d lookAxisEcef;
        lookAxisEcef = D_i_e * lookAxisJ2000;
        //std::cout<<"Look ECEF: "<<lookAxisEcef<<std::endl;

        // Calcualte the location of the ground target in ECEF
        Eigen::Vector3d S(posEcef_);

        // We are going to use law of cosines to solve the triangle that connects
        // S: The vector from earth center to the satellite
        // R: The vector from the earth center to the point on the earth the camera is pointing at
        // L: The line of sight vector of the satellites zaxis 
        

        // S is the magnitude of the satellites position vector in ECEF,
        // unit(L) can be determined from the satellites quaternion measurement
        // The angle between -S and L can be calculated using the dot product
        double alpha, dot;
        dot = -S.dot( lookAxisEcef );
        alpha = acos( dot / S.norm() );
        
        // If alpha is the angle between -S and L 
        // then the law of cosines states that
        // note: all values in this comment below are scalar vector magnitudes or angles
        // R^2 = S^2 + L^2 - 2*S*L*cos( alpha )
        // R is the earths radius - assume spherical 6378 km 
        // S is known from the GPS position of the satellite.
        // L is unknown. We can rearrange this problem into quadratic form
        // 0 = L^2 - 2*S*cos(alpha)*L + ( S^2 - R^2 )
        //      a       b                c
        // and solve using the quadratic equation
        // L =  ( -b +/- sqrt( b*b - 4*a*c ) ) / ( 2* a)
        // a = 1
        // b = -2*S*cos(alpha)
        // c = (S^2 - R^2 )
        double a(1.0);
        double b(0.0);
        double c(0.0);
        b = -2 * S.norm() * cos( alpha );
        c = pow( S.norm() ,2 ) - pow( R_E, 2 );
        double sq;
        sq = b*b - 4*a*c;
        double Lmag(0.0);
        double root1(0.0);
        double root2(0.0);
        if( sq >= 0 )
        {
            // its pointed at the earth.
            root1 = (-b + sqrt( b*b - 4.0*a*c )) / ( 2.0 * a );
            root2 = (-b - sqrt( b*b - 4.0*a*c )) / ( 2.0 * a );
            // the smallest one is 
            Lmag = std::min( root1, root2 );
        }
        else
        {
            // the problem isnt solvable - we arent pointed at earth at all. you are gucci
            return false;
        }
        // Now that we have calculated the magnitude of L we know the whole problem geometry and can calculate the
        // position where the satellites line of sight intersects the earth using the parallelagaram rule.
        // R = 
        Eigen::Vector3d L;
        Eigen::Vector3d lookEcef;
        L = lookAxisEcef * Lmag;
        lookEcef = S + L;

        // Convert ECEF ground target to Latitude and Longitude
        double lat;
        double lon;
        double h;
        h = sqrt( lookEcef.x()*lookEcef.x() + lookEcef.y()*lookEcef.y() );
        lat = atan(  lookEcef.z() / h) * 180.0 / M_PI ;
        lon = atan2( lookEcef.y() , lookEcef.x() ) * 180.0 / M_PI ;
        Point groundTarget;
        lon = fmod( lon ,360.0);
        if( lon < 0.0 )
        {
            lon += 360.0;
        }
        groundTarget.h = lon;
        groundTarget.v = lat;
        // Iterate over all the zones in the restricted list
        for( auto zone : restricted_ )
        {
            if( zone.isInside( groundTarget ) )
            {
                rejected_ = zone.name();
                return true;
            }
        }
        return false;
    }
    std::string Geofence::rejected_zone()
    {
        return rejected_;
    }


}