#include "TimeScale.hpp"
#include <cmath>
namespace cromulence
{
        // this function finds the julian date given the year, month, day, and time.
    //the julian date is defined by each elapsed day since noon, jan 1, 4713 bc.
    // Adapted from Vallado
    //inputs          description                    range / units
    //  year        - year                           1900 .. 2100
    //  mon         - month                          1 .. 12
    //  day         - day                            1 .. 28,29,30,31
    //  hr          - universal time hour            0 .. 23
    //  min         - universal time min             0 .. 59
    //  sec         - universal time sec             0.0 .. 59.999
    //  whichtype   - julian or gregorian calender   'j' or 'g'
    JulianDate julian_day(Gregorian t )
    {   
        double jd =  367.0 * t.yr  
             - floor( (7 * (t.yr + floor( (t.mo + 9) / 12.0) ) ) * 0.25 )   
             + floor( 275 * t.mo / 9.0 ) 
             + t.day + 1721013.5;  //use - 678987.0 to go to mjd directly
        double jdfrac = (t.sec + t.min * 60.0 + t.hr *3600.0) / 86400.0;
        JulianDate out;
        out.days = jd;
        out.frac = jdfrac;
        return out;
    
    }
    hms sec_to_hms( double sec )
    {
        hms out;
        double temp  = sec / 3600.0;
        out.hr    = static_cast<int>( temp );
        out.min   =  static_cast<int>( (temp - out.hr)* 60.0 );
        out.sec   = (temp - out.hr - out.min/60.0 ) * 3600.0;


        return out;
    }

    double hms_to_sec( const hms in )
    {
        return in.hr*3600.0 + in.min*60.0 + in.sec;
    }
    TimeScale::TimeScale() :
        Dat_(37.0),
        Dut_(0.0)
    {

    }

    TimeScale::~TimeScale()
    {

    }

    void TimeScale::setOffsets( double deltaUT , double deltaAt )
    {
        Dat_ = deltaAt;
        Dut_ = deltaUT;
    }
    AstronomicalTime TimeScale::from_utc( Gregorian utc )
    {
        AstronomicalTime out;
        out.jd_utc =  julian_day( utc );
        out.mjd  = out.jd_utc.days +out.jd_utc.frac - 2400000.5;
        hms utc_hms;
        utc_hms.hr = utc.hr;
        utc_hms.min = utc.min;
        utc_hms.sec = utc.sec;
        out.utc = hms_to_sec( utc_hms );

        out.ut1= out.utc + Dut_;
        Gregorian temp( utc );
        hms temp_time;
        temp_time = sec_to_hms(  out.ut1 );
        temp.hr = temp_time.hr;
        temp.min = temp_time.min;
        temp.sec= temp_time.sec;
        out.jd_ut1 = julian_day( temp );
        //tut1= (out.jd_ut1.days+ out.jd_ut1.frac - 2451545.0  )/ 36525.0;

        out.tai= out.utc + Dat_;
        temp_time = sec_to_hms(  out.tai );
        temp.hr = temp_time.hr;
        temp.min = temp_time.min;
        temp.sec= temp_time.sec;
        out.jd_tai = julian_day(temp);

        out.tt= out.tai + 32.184; // sec 
        temp_time = sec_to_hms( out.tt );
        temp.hr = temp_time.hr;
        temp.min = temp_time.min;
        temp.sec= temp_time.sec;
        out.jd_tt = julian_day(temp);
        out.ttt= (out.jd_tt.days+out.jd_tt.frac - 2451545.0  )/ 36525.0;

        out.tdb = out.tt + 0.001657*sin(628.3076*out.ttt+6.2401) 
               + 0.000022*sin(575.3385*out.ttt+4.2970) 
               + 0.000014*sin(1256.6152*out.ttt+6.1969) 
               + 0.000005*sin(606.9777*out.ttt+4.0212) 
               + 0.000005*sin(52.9691*out.ttt+0.4444) 
               + 0.000002*sin(21.3299*out.ttt+5.5431) 
               + 0.000010*out.ttt*sin(628.3076*out.ttt+4.2490);  
        temp_time = sec_to_hms( out.tdb );
        temp.hr = temp_time.hr;
        temp.min = temp_time.min;
        temp.sec= temp_time.sec;
        out.jd_tdb = julian_day( temp );
        out.ttdb = (out.jd_tdb.days + out.jd_tdb.frac - 2451545.0  )/ 36525.0;
        return out;
    }
}