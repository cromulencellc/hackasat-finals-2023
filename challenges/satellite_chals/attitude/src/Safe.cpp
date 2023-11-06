#include "Safe.hpp"


namespace cromulence::attitude
{

    Safe::Safe( )
    {

    }
    Safe::~Safe()
    {

    }

    void  Safe::setTime( cromulence::Gregorian utc )
    {
        cromulence::AstronomicalTime astro;
        astro = ts.from_utc( utc );
        sol_.setTime( astro );
        losEarthToSol_ = sol_.eci_position();
        losEarthToSol_.normalize();
    }

    void  Safe::setPosition( double x, double y, double z)
    {
        nadirEci_ <<-x,-y,-z;
        nadirEci_.normalize();
    }

    Eigen::Quaterniond Safe::safe_quaternion( )
    {

        // The moonlighter star tracker is 
        //   60 degrees off the -y axis
        //   30 degrees off the -z axiss

        // We need to keep the star tracker 
        // 135 degree half angle clear of earth
        // 90 degree half angle clear of sun
        // This is not always possible but our strategy is this

        // point -y anti nadir
        // try to rotate about -y such that we have maximum sun clearance
        //  
        Eigen::Vector3d yAxisEci =nadirEci_;
        Eigen::Vector3d xAxisEci, zAxisEci;
        xAxisEci = -losEarthToSol_.cross( yAxisEci );
        zAxisEci = xAxisEci.cross( zAxisEci );

        Eigen::Matrix3d rot;

        rot << xAxisEci[0],xAxisEci[1],xAxisEci[2],
               yAxisEci[0],yAxisEci[1],yAxisEci[2],
               zAxisEci[0],zAxisEci[1],zAxisEci[2];
        Eigen::Quaterniond out( rot );
        return out;
    }

}