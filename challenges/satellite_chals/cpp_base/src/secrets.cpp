#include "secrets.hpp"

namespace cromulence
{

    namespace secrets
    {
        void readSecret( std::string envfile , int &out )
        {
            std::string str;
            char *end;
            str = readSecretToStr( envfile);
            if( str == "unset ")
            {
                out = 0;
            }
            else
            {
                out = std::strtod( str.c_str() , &end);
            }
            
        }
        std::string readSecretToStr( std::string envFile )
        {
            std::string item_name;
            std::string out;
            std::ifstream nameFileout;
            nameFileout.open(envFile , std::ifstream::in ) ;
            if( nameFileout.is_open() )
            {
                std::getline(nameFileout, out);
                nameFileout.close();
                std::filesystem::remove(envFile);
            }
            else
            {
                out = "unset";
            }
            return out;
        }

    }
}