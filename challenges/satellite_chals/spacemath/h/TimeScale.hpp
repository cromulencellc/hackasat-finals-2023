#pragma once


namespace cromulence
{
    struct Gregorian
    {
        int yr;
        int mo;
        int day;
        int hr;
        int min;
        double sec;
    };
    struct JulianDate
    {
        double days;
        double frac;
    };
    struct hms
    {
        int hr;
        int min;
        double sec;
    };
    struct AstronomicalTime
    {
        double ut1;
        double tut1;
        double mjd;
        JulianDate jd_ut1;
        JulianDate jd_utc;
        double utc;
        double tai;
        JulianDate jd_tai;
        double tt;
        double ttt;
        JulianDate jd_tt;
        double tdb;
        double ttdb;
        JulianDate jd_tdb;
    };

    hms sec_to_hms( const double sec );
    double hms_to_sec( const hms in );
    JulianDate julian_day(Gregorian time );

    class TimeScale
    {
    public:
        TimeScale( );
        ~TimeScale();

        void setOffsets( double deltaUT , double deltaAt );

        AstronomicalTime from_utc(Gregorian utc);
    protected:
        double Dat_;
        double Dut_;
    };

}