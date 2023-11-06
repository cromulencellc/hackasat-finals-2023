#include <fstream>
#include <filesystem>


namespace cromulence
{
    namespace secrets
    {
            void readSecret( std::string path , int &out);
            std::string readSecretToStr( std::string path);
    }

}