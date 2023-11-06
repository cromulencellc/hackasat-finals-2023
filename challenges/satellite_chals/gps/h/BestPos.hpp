#pragma once
#include "LogParser.hpp"
#include <stdexcept>
#include <string>     // std::string,
#include <vector>
#include "GpsMessages.hpp"
#include "Earth.hpp"
#include "str.hpp"
namespace cromulence::gps
{

    class BestPos : public NovatelLogParser
    {
    public:
    BestPos()
    {
    }
    ~BestPos()
    {
    }

    void to_message(std::vector<std::string> &msg)
    {

        //<OK[COM1]#BESTPOSA,COM1,0,91.0,FINESTEERING,2273,426019.000,02004024,cdba,16410;SOL_COMPUTED,SINGLE,33.85946793963,-163.77572990279,408806.9565,-12.4000,WGS84,2.1413,3.0728,4.5220,"",0.000,0.000,10,10,10,0,00,02,00,01
        //0: #BESTPOSA, 
        //1: COM1, 
        //2: 0, 
        //3: 78.5,
        //4: FINESTEERING,
        //5: 1419,
        //6: 336208.000,
        //7: 02000040,
        //8: 6145,
        //9: 2724;SOL_COMPUTED,
        //10: NARROW_INT,
        //11:: 51.11635910984,
        //12: -114.03833105168,
        //13: 1063.8416,
        //14: -16.2712,
        //15: WGS84,
        //16: 0.0135,  // std-dev
        //17: 0.0084,
        //18: 0.0172,
        //19: "AAAA", //stnid
        //20: 1.000,
        //21: 0.000, //age
        //22: 8, // #sv
        //23: 8, // #sv in sol
        //24: 8,
        //25: 8,
        //26: 0,
        //27: 01,
        //28: 0,
        //29 03
        if (msg.size() != 30)
        {
            throw std::range_error("Bad sized BESTPOSA" );
        }
        double latitude = std::stod(msg[11]);
        double longitude = std::stod(msg[12]);
        double altitude = std::stod(msg[13]) / 1000.0; // convert to km
        bool valid( false );
        CFE_ES_WriteToSysLog( "GPS BEST POS %s %s ", msg[9], msg[15].c_str() );

        // VULN ADDED: If we really wanted to be good we should be checking msg[9] has "SOL_COMPUTED" in it
        // VULN ADDED: We should be checking htat msg[15] is WGS84 
        
        //valid = msg[9].find("SOL_COMPUTED") != std::string::npos; 
        valid = true;
        if( true == valid )
        {
            GeodeticMsg g;
            g.data.payload.latitude = latitude;
            g.data.payload.longitude = longitude;
            g.data.payload.altitude = altitude;
            // deviations in km
            g.data.payload.dev_lat = std::stod( msg[16] ) / 1000.0;
            g.data.payload.dev_lon = std::stod( msg[17] ) / 1000.0;
            g.data.payload.dev_alt = std::stod( msg[18] ) / 1000.0;
            // 
            g.data.payload.sats_tracked = std::stoi( msg[22]);
            g.data.payload.sats_soln = std::stoi( msg[23]);
            g.data.payload.soln_age = std::stod(  msg[21]);

            cromulence::strncpy_nt( g.data.payload.datum , msg[15].c_str() , DATUM_LEN );
            cromulence::strncpy_nt( g.data.payload.solution_info , msg[9].c_str() , SOLN_LEN );
            g.send();

            EcefMsg e;
            auto o = earth_.llh_to_ecef( latitude, longitude, altitude );

            e.data.payload.position[0] = o[0];
            e.data.payload.position[1] = o[1];
            e.data.payload.position[2] = o[2];
            // if this is sume
            e.send();
        }
        else
        {
            CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "GPS rejcted ");
        }

    }

    cromulence::Earth earth_;
    };
}