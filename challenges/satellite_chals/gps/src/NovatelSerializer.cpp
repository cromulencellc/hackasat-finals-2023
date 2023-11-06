#include "NovatelSerializer.hpp"
#include "BestPos.hpp"
#include <regex>

extern "C" {
   #include "cfe.h"
}
using namespace std::literals;

namespace cromulence::gps
{

    NovatelDeSerializer::NovatelDeSerializer()
    {
        strVec_.reserve( 100 );
        map_.insert( {"BESTPOSA"s, reinterpret_cast<NovatelLogParser*>(&bestpos_ )});
        map_.insert( {"#BESTPOSA"s, reinterpret_cast<NovatelLogParser*>(&bestpos_ )});

    }
    NovatelDeSerializer::~NovatelDeSerializer()
    {
        
    }

    void NovatelDeSerializer::to_message(std::string message)
    {
        try
        {
            strVec_.clear();
            // split on ","
            std::regex reg(",");
            std::copy(
            std::sregex_token_iterator(message.begin(), message.end(), reg, -1),
            std::sregex_token_iterator(),
            std::back_inserter( strVec_) ) ;
            // The first object is the id
            std::string id;
            id = strVec_[0];
            // look it up in the map
            if( map_.contains( id ) )
            {
                NovatelLogParser* parser;
                parser = map_[id];
                parser->to_message( strVec_ );
            }
            else
            {
                CFE_EVS_SendEvent(0, CFE_EVS_CRITICAL, "Bad ID: %s ", id.c_str() );
            }
        
        }
        catch(const std::exception& e)
        {
            CFE_EVS_SendEvent(0, CFE_EVS_CRITICAL, "Sketchy gps meessage received " );

        }
        
    }


}
