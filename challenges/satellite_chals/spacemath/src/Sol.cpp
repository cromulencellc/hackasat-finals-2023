#include "Sol.hpp"


namespace cromulence
{
    Sol::Sol()
    {

    }

    Sol::~Sol()
    {

    }

    Eigen::Vector3d Sol::eci_position( )
    {
        Eigen::Vector3d out;
        out = sunPositionAu * KM_PER_AU;
        return out;
    }

    void Sol::setTime( const AstronomicalTime &t)
    {
        
        double tut1;
        double jd;
        jd = t.jd_utc.days + t.jd_utc.frac;
        tut1= ( jd - 2451545.0  )/ 36525.0;

        double meanlong;
        meanlong= 280.460  + 36000.77*tut1;
        meanlong= std::remainder( meanlong,360.0  ); // defrees
        double ttdb;
        ttdb= tut1;
        double meananomaly;
        meananomaly= 357.5277233  + 35999.05034 *ttdb;
        meananomaly= std::remainder( meananomaly*deg2rad, 2.0*M_PI );  //radians
        if ( meananomaly < 0.0  )
        {
            meananomaly= 2.0*M_PI + meananomaly;
        }
        double eclplong;
        eclplong= meanlong + 1.914666471 *sin(meananomaly)
                    + 0.019994643 *sin(2.0 *meananomaly);//deg
        eclplong= std::remainder( eclplong,360.0  );  //deg
        double obliquity;
        obliquity= 23.439291  - 0.0130042 *ttdb; //deg

        eclplong = eclplong *deg2rad;
        obliquity= obliquity *deg2rad;

       // find magnitude of sun vector, )   
       double magr;
       magr= 1.000140612  - 0.016708617 *cos( meananomaly ) 
              - 0.000139589 *cos( 2.0 *meananomaly );   // AU 

        sunPositionAu[0]= magr*cos( eclplong );
        sunPositionAu[1]= magr*cos(obliquity)*sin(eclplong);
        sunPositionAu[2]= magr*sin(obliquity)*sin(eclplong);

    }
}