#pragma once
#include <vector>
#include <string>
#include <map>
#include "BestPos.hpp"
namespace cromulence::gps
{
    class NovatelLogParser
    {
    public:
        NovatelLogParser(){}
        virtual ~NovatelLogParser(){}
        virtual void to_message( std::vector<std::string> &v )=0;
    };
}