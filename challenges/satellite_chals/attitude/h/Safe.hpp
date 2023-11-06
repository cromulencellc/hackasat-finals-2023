#pragma once
#include "TimeScale.hpp"
#include "Earth.hpp"
#include "Sol.hpp"
#include <eigen3/Eigen/Dense>
namespace cromulence::attitude
{
    // IMPORTANT THIS APP HAS HARD BAKED ASSUMPTIONS ABOUT THE MOONLIGHER SATELLITE
    class Safe
    {
    public:
        Safe();
        ~Safe();

        void setTime( cromulence::Gregorian utc );
        void setPosition( double x, double y, double z);

        Eigen::Quaterniond safe_quaternion( );
    protected:

        Eigen::Vector3d losEarthToSol_;
        Eigen::Vector3d nadirEci_;
        cromulence::TimeScale ts;
        cromulence::Sol sol_;

    
    };

}