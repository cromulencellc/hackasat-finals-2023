#include "IronBank.hpp"
#include <iostream>
#include <stdexcept>

int main( void )
{
    int ec(0);
    std::cout<<"The iron bank will have its due"<<std::endl;
    cromulence::ironbank::Vault vault( "./cf/ironbank.json" ,"./cf/ironbank.users");
    std::cout<<"ironbank online"<<std::endl;
    try
    {
        vault.execute() ;
    }
    catch(const std::exception& e)
    {
        ec = -1;
    }
    std::cout<<"Done"<<std::endl;
    return ec;
    
}