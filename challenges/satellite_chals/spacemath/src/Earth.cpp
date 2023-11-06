#include "Earth.hpp"

namespace cromulence
{

    Earth::Earth()
    {

    }
    Earth::~Earth()
    {

    }
    Eigen::Matrix3d Earth::j2000_to_fixed( AstronomicalTime t )
    {
        //TODO FIXME
        double xp(0.0), yp(0.0); // We're going to assume these as zero. Should result in errors on order of 10s of meters for the pole.
        Eigen::Matrix3d ecef_to_eci;
        Iau2006Args( t );
        Eigen::Matrix3d prec = precess ( t );
        Eigen::Matrix3d nut  = nutation( t);
        Eigen::Matrix3d st   = sidereal(  t);
        Eigen::Matrix3d pm   = polarmotion( t , xp , yp );

        Eigen::Matrix3d dcm_e_2_i;
        dcm_e_2_i = prec * nut * st * pm ;
        return dcm_e_2_i.transpose();


    }
    Eigen::Vector3d Earth::llh_to_ecef( double latDeg, double lonDeg , double altKm)
    {
        double x,y,z;
        double s;
        double c;
        double clon,clat,slon,slat;
        clon = cos( deg2rad * lonDeg );
        slon = sin( deg2rad * lonDeg );
        clat = cos( deg2rad * latDeg );
        slat = sin( deg2rad * latDeg );
        c = 1 / sqrt(  (clat * clat) + ((1 - flat) * (1 - flat) * slat * slat) );
        s = (1-flat) * (1-flat )* c;
        x = ( radius*c + altKm ) * clat * clon;
        y = ( radius*c + altKm ) * clat *slon;
        z = ( radius*s + altKm ) * slat; 
        Eigen::Vector3d out;
        out << x,y,z;
        return out;
    }

    void Earth::Iau2006Args( AstronomicalTime ts )
    {
        double ttt(ts.ttt);
        double ttt2( ttt * ttt);
        double ttt3( ttt*ttt*ttt);
        double ttt4( ttt*ttt*ttt*ttt);
        double l,l1,f,d,omega;
        l =  134.96340251  + ( 1717915923.2178 *ttt + 
             31.8792 *ttt2 + 0.051635 *ttt3 - 0.00024470 *ttt4 ) / 3600.0;
        l1   =  357.52910918  + (  129596581.0481 *ttt - 
                 0.5532 *ttt2 + 0.000136 *ttt3 - 0.00001149*ttt4 )  / 3600.0;
        f    =   93.27209062  + ( 1739527262.8478 *ttt -
                     12.7512 *ttt2 - 0.001037 *ttt3 + 0.00000417*ttt4 )  / 3600.0;
        d    =  297.85019547  + ( 1602961601.2090 *ttt - 
                  6.3706 *ttt2 + 0.006593 *ttt3 - 0.00003169*ttt4 )  / 3600.0;
        omega=  125.04455501  + (   -6962890.5431 *ttt + 
                  7.4722 *ttt2 + 0.007702 *ttt3 - 0.00005939*ttt4 )  / 3600.0;

        // conversions and remainder
        l_    = std::remainder( l,360.0  )     * deg2rad;
        ll_   = std::remainder( l1,360.0  )    * deg2rad;
        f_    = std::remainder( f,360.0  )     * deg2rad;
        d_    = std::remainder( d,360.0  )     * deg2rad;
        omega_= std::remainder( omega,360.0  ) * deg2rad;

        // tabular components
                // tabular componentes
        double deltapsi= 0.0;
        double deltaeps= 0.0;
        for( int k= 106; k < 106 ; k++ )
        {
            int i=  105 - k;
            double tempval(0.0);
            tempval= IAR[i][0]*l_ + IAR[i][1]*ll_ + IAR[i][2]*f_ + 
                     IAR[i][3]*d_ + IAR[i][4]*omega_;
            deltapsi= deltapsi + (RAR[i][0]+RAR[i][1]*ttt) * sin( tempval );
            deltaeps= deltaeps + (RAR[i][2]+RAR[i][3]*ttt) * cos( tempval );
        }
        delta_eps_ = deltaeps;
        delta_psi_ = deltapsi;
        
    }

    // Based on vallados matlab implementation from celestrak
    // ONLY IAU06 model
    Eigen::Matrix3d Earth::precess( AstronomicalTime ts)
    {
        //function [prec,psia,wa,ea,xa] = precess ( ttt, opt );
        double convrt = M_PI / (180.0*3600.0);
        //double ttt2= ts.ttt * ts.ttt;
        //double ttt3= ttt2 * ts.ttt;

        Eigen::Matrix3d prec;
        double ttt = ts.ttt;
    
        //------------------ iau 06 precession angles -------------------
        double oblo =  84381.406;
        double psia =  (((( -0.0000000951 * ttt + 0.000132851 ) * ttt - 0.00114045 ) * ttt - 1.0790069 ) * ttt + 5038.481507 ) * ttt; 
        double wa   =  ((((  0.0000003337 * ttt - 0.000000467 ) * ttt - 0.00772503 ) * ttt + 0.0512623 ) * ttt -    0.025754 ) * ttt + oblo;
        double ea   =  (((( -0.0000000434 * ttt - 0.000000576 ) * ttt + 0.00200340 ) * ttt - 0.0001831 ) * ttt -   46.836769 ) * ttt + oblo;
        double xa   =  (((( -0.0000000560 * ttt + 0.000170663 ) * ttt - 0.00121197 ) * ttt - 2.3814292 ) * ttt +   10.556403 ) * ttt;
        double zeta =  (((( -0.0000003173 * ttt - 0.000005971 ) * ttt + 0.01801828 ) * ttt + 0.2988499 ) * ttt + 2306.083227 ) * ttt + 2.650545; 
        double theta=  (((( -0.0000001274 * ttt - 0.000007089 ) * ttt - 0.04182264 ) * ttt - 0.4294934 ) * ttt + 2004.191903 ) * ttt;
        double z    =  ((((  0.0000002904 * ttt - 0.000028596 ) * ttt + 0.01826837 ) * ttt + 1.0927348 ) * ttt + 2306.077181 ) * ttt - 2.650545;
        // convert units to radians
        psia = psia  * convrt; 
        wa   = wa    * convrt;
        ea   = ea    * convrt;
        xa   = xa    * convrt;
        zeta = zeta  * convrt;
        theta= theta * convrt;
        z    = z     * convrt;

        // calculate the matrix terms
        double coszeta  = cos(zeta);
        double sinzeta  = sin(zeta);
        double costheta = cos(theta);
        double sintheta = sin(theta);
        double cosz     = cos(z);
        double sinz     = sin(z);

        prec(0,0) =  coszeta * costheta * cosz - sinzeta * sinz;
        prec(0,1) =  coszeta * costheta * sinz + sinzeta * cosz;
        prec(0,2) =  coszeta * sintheta;
        prec(1,0) = -sinzeta * costheta * cosz - coszeta * sinz;
        prec(1,1) = -sinzeta * costheta * sinz + coszeta * cosz;
        prec(1,2) = -sinzeta * sintheta;
        prec(2,0) = -sintheta * cosz;
        prec(2,1) = -sintheta * sinz;
        prec(2,2) =  costheta;


        return prec;
    }
    Eigen::Matrix3d Earth::sidereal(AstronomicalTime ts )
    {
        

        //------------------------ find gmst --------------------------
        double gmst= gstime( ts);
        /// mean eps
        double meaneps = mean_eps( ts);
        double om = omega( ts.ttt );
        double ast(0.0);
        //% ------------------------ find mean ast ----------------------
        // after 1997, kinematic terms apply as well as gemoetric in eqe
        double jdut1;
        jdut1 = ts.jd_ut1.days +ts.jd_ut1.frac;
        if (jdut1 > 2450449.5 ) 
        {
            ast= gmst + delta_psi_* cos(meaneps)
             + 0.00264*M_PI /(3600.0*180.0)*sin(om) 
             + 0.000063*M_PI /(3600.0*180.0)*sin(2.0 *om);
        }
        else
        {
            ast= gmst + delta_psi_* cos(meaneps);
        }
        ast = std::remainder(ast, 2.0*M_PI);

        Eigen::Matrix3d st;
        st(0,0) =  cos(ast);
        st(0,1) = -sin(ast);
        st(0,2) =  0.0;
        st(1,0) =  sin(ast);
        st(1,1) =  cos(ast);
        st(1,2) =  0.0;
        st(2,0) =  0.0;
        st(2,1) =  0.0;
        st(2,2) =  1.0;

        return st;
    }
    Eigen::Matrix3d Earth::nutation( AstronomicalTime ts)
    {
        // ---- determine coefficients for iau 1980 nutation theory ----
        double meaneps = mean_eps( ts );
        

        // --------------- find nutation parameters --------------------
        double deltapsi(0.0);
        double deltaeps(0.0);
        double ddpsi(0.0); //assume as 0.0 for now
        double ddeps(0.0); //assume as 0.0 for now
        deltapsi = std::remainder( delta_psi_ + ddpsi, 2.0 * M_PI );
        deltaeps = std::remainder( delta_eps_ + ddeps, 2.0 * M_PI );
        double trueeps  = meaneps + deltaeps;

        // make matrix terms
        double cospsi  = cos(deltapsi);
        double sinpsi  = sin(deltapsi);
        double coseps  = cos(meaneps);
        double sineps  = sin(meaneps);
        double costrueeps = cos(trueeps);
        double sintrueeps = sin(trueeps);
        // Construct the matrix
        Eigen::Matrix3d nut;
        nut(0,0) =  cospsi;
        nut(0,1) =  costrueeps * sinpsi;
        nut(0,2) =  sintrueeps * sinpsi;
        nut(1,0) = -coseps * sinpsi;
        nut(1,1) =  costrueeps * coseps * cospsi + sintrueeps * sineps;
        nut(1,2) =  sintrueeps * coseps * cospsi - sineps * costrueeps;
        nut(2,0) = -sineps * sinpsi;
        nut(2,1) =  costrueeps * sineps * cospsi - sintrueeps * coseps;
        nut(2,2) =  sintrueeps * sineps * cospsi + costrueeps * coseps;

        return nut;

    }
    Eigen::Matrix3d Earth::polarmotion( AstronomicalTime t,  double xp, double yp)
    {

        double cosxp = cos(xp);
        double sinxp = sin(xp);
        double cosyp = cos(yp);
        double sinyp = sin(yp);


        double convrt = M_PI / (3600.0*180.0);
        // approximate sp value in rad
        double sp = -47.0e-6 * t.ttt * convrt;
        double cossp = cos(sp);
        double sinsp = sin(sp);
        Eigen::Matrix3d pm;
        pm(0,0) =  cosxp * cossp;
        pm(0,1) = -cosyp * sinsp + sinyp * sinxp * cossp;
        pm(0,2) = -sinyp * sinsp - cosyp * sinxp * cossp;
        pm(1,0) =  cosxp * sinsp;
        pm(1,1) =  cosyp * cossp + sinyp * sinxp * sinsp;
        pm(1,2) =  sinyp * cossp - cosyp * sinxp * sinsp;
        pm(2,0) =  sinxp;
        pm(2,1) = -sinyp * cosxp;
        pm(2,2) =  cosyp * cosxp;
        return pm;
        
    }
    double Earth::mean_eps( AstronomicalTime ts )
    {
        double meaneps;

        // ---- determine coefficients for iau 1980 nutation theory ----
        double ttt2= ts.ttt*ts.ttt;
        double ttt3= ttt2*ts.ttt;

        meaneps = -46.8150 *ts.ttt - 0.00059 *ttt2 + 0.001813 *ttt3 + 84381.448;
        meaneps = std::remainder( meaneps/3600.0, 360.0 );
        meaneps = meaneps * deg2rad;
        return meaneps;
    }
    double Earth::gstime(AstronomicalTime ts)
    {
        double jdut1 = ts.jd_ut1.days + ts.jd_utc.frac;
        double tut1= ( jdut1 - 2451545.0 ) / 36525.0;

        double temp = - 6.2e-6 * tut1 * tut1 * tut1 + 0.093104 * tut1 * tut1 + (876600.0 * 3600.0 + 8640184.812866) * tut1 + 67310.54841;

        temp = std::remainder( temp*deg2rad/240.0, 2.0*M_PI );

        if ( temp < 0.0 )
        {
            temp = temp + (2.0*M_PI);
        }
        return temp;
    }

    const int8_t Earth::IAR[106][5] = { {0,0,0,0,1},
            {0,0,2,-2,2},
            {0,0,2,0,2},
            {0,0,0,0,2},
            {0,1,0,0,0},
            {1,0,0,0,0},
            {0,1,2,-2,2},
            {0,0,2,0,1},
            {1,0,2,0,2},
            {0,-1,2,-2,2},
            {1,0,0,-2,0},
            {0,0,2,-2,1},
            {-1,0,2,0,2},
            {1,0,0,0,1},
            {0,0,0,2,0},
            {-1,0,2,2,2},
            {-1,0,0,0,1},
            {1,0,2,0,1},
            {2,0,0,-2,0},
            {-2,0,2,0,1},
            {0,0,2,2,2},
            {2,0,2,0,2},
            {2,0,0,0,0},
            {1,0,2,-2,2},
            {0,0,2,0,0},
            {0,0,2,-2,0},
            {-1,0,2,0,1},
            {0,2,0,0,0},
            {0,2,2,-2,2},
            {-1,0,0,2,1},
            {0,1,0,0,1},
            {1,0,0,-2,1},
            {0,-1,0,0,1},
            {2,0,-2,0,0},
            {-1,0,2,2,1},
            {1,0,2,2,2},
            {0,-1,2,0,2},
            {0,0,2,2,1},
            {1,1,0,-2,0},
            {0,1,2,0,2},
            {-2,0,0,2,1},
            {0,0,0,2,1},
            {2,0,2,-2,2},
            {1,0,0,2,0},
            {1,0,2,-2,1},
            {0,0,0,-2,1},
            {0,-1,2,-2,1},
            {2,0,2,0,1},
            {1,-1,0,0,0},
            {1,0,0,-1,0},
            {0,0,0,1,0},
            {0,1,0,-2,0},
            {1,0,-2,0,0},
            {2,0,0,-2,1},
            {0,1,2,-2,1},
            {1,1,0,0,0},
            {1,-1,0,-1,0},
            {-1,-1,2,2,2},
            {0,-1,2,2,2},
            {1,-1,2,0,2},
            {3,0,2,0,2},
            {-2,0,2,0,2},
            {1,0,2,0,0},
            {-1,0,2,4,2},
            {1,0,0,0,2},
            {-1,0,2,-2,1},
            {0,-2,2,-2,1},
            {-2,0,0,0,1},
            {2,0,0,0,1},
            {3,0,0,0,0},
            {1,1,2,0,2},
            {0,0,2,1,2},
            {1,0,0,2,1},
            {1,0,2,2,1},
            {1,1,0,-2,1},
            {0,1,0,2,0},
            {0,1,2,-2,0},
            {0,1,-2,2,0},
            {1,0,-2,2,0},
            {1,0,-2,-2,0},
            {1,0,2,-2,0},
            {1,0,0,-4,0},
            {2,0,0,-4,0},
            {0,0,2,4,2},
            {0,0,2,-1,2},
            {-2,0,2,4,2},
            {2,0,2,2,2},
            {0,-1,2,0,1},
            {0,0,-2,0,1},
            {0,0,4,-2,2},
            {0,1,0,0,2},
            {1,1,2,-2,2},
            {3,0,2,-2,2},
            {-2,0,2,2,2},
            {-1,0,0,0,2},
            {0,0,-2,2,1},
            {0,1,2,0,1},
            {-1,0,4,0,2},
            {2,1,0,-2,0},
            {2,0,0,2,0},
            {2,0,2,-2,1},
            {2,0,-2,0,1},
            {1,-1,0,-2,0},
            {-1,0,0,1,1},
            {-1,-1,0,2,1},
            {0,1,0,1,0} };
    const double Earth::RAR[106][4] = {
        {-8.3386e-05,-8.44545e-08,4.4615e-05,4.31484e-09},
        {-6.39324e-06,-7.75702e-10,2.78089e-06,-1.50292e-09},
        {-1.10247e-06,-9.69627e-11,4.73663e-07,-2.42407e-10},
        {9.99686e-07,9.69627e-11,-4.33908e-07,2.42407e-10},
        {6.91344e-07,-1.64837e-09,2.61799e-08,-4.84814e-11},
        {3.45187e-07,4.84814e-11,-3.3937e-09,0},
        {-2.50649e-07,5.81776e-10,1.08598e-07,-2.90888e-10},
        {-1.87138e-07,-1.93925e-10,9.69627e-08,0},
        {-1.45929e-07,0,6.2541e-08,-4.84814e-11},
        {1.05205e-07,-2.42407e-10,-4.60573e-08,1.45444e-10},
        {-7.66006e-08,0,-4.84814e-10,0},
        {6.2541e-08,4.84814e-11,-3.3937e-08,0},
        {5.96321e-08,0,-2.56951e-08,0},
        {3.05433e-08,4.84814e-11,-1.59989e-08,0},
        {3.05433e-08,0,-9.69627e-10,0},
        {-2.8604e-08,0,1.26052e-08,0},
        {-2.81192e-08,-4.84814e-11,1.5514e-08,0},
        {-2.47255e-08,0,1.309e-08,0},
        {2.32711e-08,0,4.84814e-10,0},
        {2.23014e-08,0,-1.16355e-08,0},
        {-1.84229e-08,0,7.75702e-09,0},
        {-1.50292e-08,0,6.30258e-09,0},
        {1.40596e-08,0,-4.84814e-10,0},
        {1.40596e-08,0,-5.81776e-09,0},
        {1.26052e-08,0,-4.84814e-10,0},
        {-1.06659e-08,0,0,0},
        {1.01811e-08,0,-4.84814e-09,0},
        {8.24183e-09,-4.84814e-11,0,0},
        {-7.75702e-09,4.84814e-11,3.3937e-09,0},
        {7.75702e-09,0,-3.87851e-09,0},
        {-7.27221e-09,0,4.36332e-09,0},
        {-6.30258e-09,0,3.3937e-09,0},
        {-5.81776e-09,0,2.90888e-09,0},
        {5.33295e-09,0,0,0},
        {-4.84814e-09,0,2.42407e-09,0},
        {-3.87851e-09,0,1.45444e-09,0},
        {-3.3937e-09,0,1.45444e-09,0},
        {-3.3937e-09,0,1.45444e-09,0},
        {-3.3937e-09,0,0,0},
        {3.3937e-09,0,-1.45444e-09,0},
        {-2.90888e-09,0,1.45444e-09,0},
        {-2.90888e-09,0,1.45444e-09,0},
        {2.90888e-09,0,-1.45444e-09,0},
        {2.90888e-09,0,0,0},
        {2.90888e-09,0,-1.45444e-09,0},
        {-2.42407e-09,0,1.45444e-09,0},
        {-2.42407e-09,0,1.45444e-09,0},
        {-2.42407e-09,0,1.45444e-09,0},
        {2.42407e-09,0,0,0},
        {-1.93925e-09,0,0,0},
        {-1.93925e-09,0,0,0},
        {-1.93925e-09,0,0,0},
        {1.93925e-09,0,0,0},
        {1.93925e-09,0,-9.69627e-10,0},
        {1.93925e-09,0,-9.69627e-10,0},
        {-1.45444e-09,0,0,0},
        {-1.45444e-09,0,0,0},
        {-1.45444e-09,0,4.84814e-10,0},
        {-1.45444e-09,0,4.84814e-10,0},
        {-1.45444e-09,0,4.84814e-10,0},
        {-1.45444e-09,0,4.84814e-10,0},
        {-1.45444e-09,0,4.84814e-10,0},
        {1.45444e-09,0,0,0},
        {-9.69627e-10,0,4.84814e-10,0},
        {-9.69627e-10,0,4.84814e-10,0},
        {-9.69627e-10,0,4.84814e-10,0},
        {-9.69627e-10,0,4.84814e-10,0},
        {-9.69627e-10,0,4.84814e-10,0},
        {9.69627e-10,0,-4.84814e-10,0},
        {9.69627e-10,0,0,0},
        {9.69627e-10,0,-4.84814e-10,0},
        {9.69627e-10,0,-4.84814e-10,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,4.84814e-10,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,4.84814e-10,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {-4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,-4.84814e-10,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,-4.84814e-10,0},
        {4.84814e-10,0,-4.84814e-10,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,-4.84814e-10,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0},
        {4.84814e-10,0,0,0}
    };

};