#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <limits>

#include "manager.h"
#include "helper.h"

Manager* manager = nullptr;

void banner(){
    for(size_t i = 0; i < 74; i++) std::cout << "=";
    std::cout << "\n\n███████╗██╗███╗   ██╗ █████╗ ██╗         ██████╗  █████╗ ███████╗███████╗" << std::endl;
    std::cout <<     "██╔════╝██║████╗  ██║██╔══██╗██║         ██╔══██╗██╔══██╗██╔════╝██╔════╝" << std::endl;
    std::cout <<     "█████╗  ██║██╔██╗ ██║███████║██║         ██████╔╝███████║███████╗███████╗" << std::endl;
    std::cout <<     "██╔══╝  ██║██║╚██╗██║██╔══██║██║         ██╔═══╝ ██╔══██║╚════██║╚════██║" << std::endl;
    std::cout <<     "██║     ██║██║ ╚████║██║  ██║███████╗    ██║     ██║  ██║███████║███████║" << std::endl;
    std::cout <<     "╚═╝     ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝    ╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝" << std::endl;
    std::cout << "\n  Welcome to FinalPass. The last password manager you'll ever need." << std::endl;
    for(size_t i = 0; i < 74; i++) std::cout << "=";
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    int team_port;
    if(argc < 2){
        std::cerr << "Must provide port number. Bailing out...";
        exit(-1);
    }
    else{
        team_port = atoi(argv[1]);
    }

    int choice = 0;
    manager = new Manager{ team_port };

    banner();

    while(true){
        menu(manager);

        std::cin >> choice;
        std::cout << std::endl;

        if( !checkInput(true) ) continue;

        if(!manager->ProcessChoice(choice)){
            LOG_ERR("Choice %d failed!\n", choice);
        }
    }

    return 0;
}