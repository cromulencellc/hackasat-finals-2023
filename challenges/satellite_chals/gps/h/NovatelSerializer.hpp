#pragma once
#include <vector>
#include <string>
#include <map>
#include "BestPos.hpp"
namespace cromulence::gps
{

    class NovatelDeSerializer 
    {
    public:
        NovatelDeSerializer();
        ~NovatelDeSerializer();

        void to_message( std::string msg );
    protected:
        std::map<std::string , NovatelLogParser*> map_;
        std::vector<std::string> strVec_;
        BestPos bestpos_;
    };

}