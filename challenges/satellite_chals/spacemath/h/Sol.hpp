#pragma once

#include "TimeScale.hpp"
#include <eigen3/Eigen/Dense>
namespace cromulence
{

    class Sol
    {
    public:
        Sol();
        ~Sol();
        void setTime( const AstronomicalTime &t );
        Eigen::Vector3d eci_position( );
    protected:
        AstronomicalTime time;
        Eigen::Vector3d sunPositionAu;
        static constexpr double deg2rad = M_PI/180.0;
        static constexpr double KM_PER_AU = 149597870.7;
    };
}