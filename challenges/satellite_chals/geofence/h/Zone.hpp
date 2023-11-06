#pragma once

#include <vector>
#include <string>
namespace cromulence
{
    struct Point
    {
        double v;
        double h;
        Point ()
        {
            v = 0;
            h = 0;
        }
        Point( double vv, double hh)
        {
            v = vv;
            h = hh;
        };
    };

    class Zone
    {
    public:
        Zone(  );
        Zone( std::string zoneName );
        ~Zone();
        bool isInside( Point testPoint ); 
        void append_point( Point newPoint );
        std::string name();

    protected:
        double angleBetween( double x1 , double y1, double x2, double y2);
        bool near( double a, double b, double tolerance);
        std::vector< Point > points_;
        std::string zoneName_;
        static constexpr double TOLERANCE = 0.01;

    };
}