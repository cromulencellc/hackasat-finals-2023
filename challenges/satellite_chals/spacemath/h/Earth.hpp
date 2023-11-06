#pragma once

#include "eigen3/Eigen/Dense"
#include <cmath>
#include "TimeScale.hpp"
namespace cromulence
{
        static constexpr double deg2rad = M_PI/180.0;




       

        static constexpr double radpday = 2.0 * M_PI * 1.002737909350795;


        // conversions from ttt ---> other stuff from vallado



    class Earth
    {
    public:
        Earth();
        ~Earth();

        Eigen::Matrix3d j2000_to_fixed( AstronomicalTime t );
        Eigen::Vector3d llh_to_ecef( double latDeg, double lonDeg , double altKm);
    protected:
        Eigen::Matrix3d precess( AstronomicalTime ts );
        Eigen::Matrix3d sidereal(AstronomicalTime ts);
        Eigen::Matrix3d nutation(AstronomicalTime ts );
        Eigen::Matrix3d polarmotion( AstronomicalTime ts,  double xp, double yp );
        double mean_eps( AstronomicalTime ts );
        double gstime(AstronomicalTime ts);
        
        void Iau2006Args( AstronomicalTime ts );
        // fundamental args
        double l_;
        double ll_;
        double f_;
        double d_;
        double omega_; 
        double delta_eps_;
        double delta_psi_;

        static constexpr double radius = 6378.1363;         // km
        static constexpr double flat       = 1.0/298.257223563;
        static constexpr double rotation_rate   = 7.292115e-5;     // rad/s 
        static constexpr double mu         = 398600.4415;      /// km3/s2
        
        static constexpr double eccearth = sqrt(2.0*flat - (flat*flat));
        static constexpr double eccearthsqrd = eccearth*eccearth;
         
        static constexpr double tusec = sqrt(radius*radius*radius/mu);
        static constexpr double tumin = tusec / 60.0;
        static constexpr double tuday = tusec / 86400.0;
        static constexpr double tudaysid = tusec / 86164.090524;
        static constexpr double mass = 5.9742e24;

        static constexpr double omegaearthradptu  = rotation_rate * tusec;
        static constexpr double omegaearthradpmin = rotation_rate * 60.0;
        constexpr double omega(int ttt) {
            return 125.04455501  + (   -6962890.5431 *ttt +7.4722 *(ttt*ttt) + 0.007702 *(ttt*ttt*ttt) - 0.00005939*(ttt*ttt*ttt*ttt) )  / 3600.0;
        }
        static const int8_t IAR[106][5];
        static const double RAR[106][4];
    };




}