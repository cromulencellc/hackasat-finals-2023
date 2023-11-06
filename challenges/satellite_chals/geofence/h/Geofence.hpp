#pragma once
#include "Zone.hpp"
#include <eigen3/Eigen/Dense>
#include "TimeScale.hpp"
#include "Earth.hpp"
namespace cromulence
{

    class Geofence
    {
    public:
        Geofence();
        ~Geofence();

        void from_json( std::string pathname );
        void setLookQuaternion( double qs , double qv[3]);
        void updateTimeOffsets( double deltaAT, double deltaUT );
        void setGpsPosition(  double pos[3] );
        void setTime( Gregorian utc );
        double getJdUtc( );
        double getTAI();
        Eigen::Vector3d getGps( );
        bool calcRestricted( );
        std::string rejected_zone();
        std::string download_path();
    protected:
        void calcLookEci();
        std::string downloadPath_;
        TimeScale ts_;
        Earth earth_;
        AstronomicalTime astroTime_;
        std::vector< Zone > restricted_;
        Eigen::Matrix3d D_i_b_;
        Eigen::Matrix3d D_i_e_;
        Eigen::Vector3d posEcef_;
        double timeEpoch_;
        std::string rejected_;
        static constexpr double R_E = 6378.0;
        static constexpr double OMEGA_EARTH = 7.2921159e-5;

        
        
    };
}