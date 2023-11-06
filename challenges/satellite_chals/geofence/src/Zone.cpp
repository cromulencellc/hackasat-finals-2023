#include "Zone.hpp"
#include <cmath>
#include <stdio.h>
namespace cromulence
{
    Zone::Zone(   ) :
        zoneName_("unnamed")
    {

    }

    Zone::Zone( std::string zoneName ) :
        zoneName_(zoneName)
    {
        
    }

    Zone::~Zone()
    {

    }
    std::string Zone::name( )
    {
        return zoneName_;
    }
    void Zone::append_point( Point newPoint )
    {
        points_.push_back( newPoint );
    }
    bool Zone::isInside( Point testPoint )
    {
        size_t n = points_.size();
        double total(0.0);

        Point p1;
        Point p2;
        for( size_t k = 0; k <n ; k++)
        { 
            p1.h = points_[k].h - testPoint.h;
            p1.v = points_[k].v - testPoint.v;
            p2.h = points_[ (k+1) % n ].h - testPoint.h;
            p2.v = points_[ (k+1) % n ].v - testPoint.v;
            total += angleBetween( p1.h, p1.v, p2.h,p2.v );
        }

        if( true == near( total , 2.0*M_PI,  TOLERANCE ))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    double Zone::angleBetween( double x1 , double y1, double x2, double y2 )
    {
        double m1,m2;
        m1 = sqrt( x1*x1 + y1*y1 );
        m2 = sqrt( x2*x2 + y2*y2 );

        double dot;
        // dot product of 1 and 2
        dot = x1*x2 + y1*y2;
        double ang;
        ang = std::acos( dot / (m1 * m2 ));
        return ang;
    }
    bool Zone::near( double a, double b , double tolerance )
    {
        return fabs( a-b ) < tolerance;
    }
    
} // namespace cromulence