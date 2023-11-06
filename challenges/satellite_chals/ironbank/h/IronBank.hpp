#pragma once
#include <string>
#include <vector>
#include "UdpServer.hpp"
#include "ib_messages.hpp"
namespace cromulence::ironbank
{



    std::string gen_random(const int len);  


    std::string join( std::vector<std::string> vec , std::string sep );

    class Vault 
    {

    public:
        Vault( std::string secrets , std::string users   );
        ~Vault();
        void execute();
        void stop( );
    protected:
        cromulence::network::UdpServer udp_;
        void issue_challenge( const command &cmd );
        bool is_protected(const command &cmd);
        void clear();
        void generate_key();
        void handle_commands();
        void auth_loop();
        void list_users();
        bool validate_key( const command &cmd );
        void protected_command( const command &cmd );
        void unprotected_command( const command &cmd);
        void download_flag( );
        void download_script( const command &cmd );
        void load_script(  const command &cmd );

        bool valid_script_id( char *cmd );
        void print_sha( char * in );
        bool user_exists( std::string in );
        void clear_run_folder( );
    private:
        size_t burn( size_t n);
        bool keepGoing_;
        bool challengeIssued_;
        std::string downloadPath_;
        std::string uploadPath_;
        std::string exePath_;
        std::string user_;
        std::string key_;
        std::string challenge_;
        std::string response_;
        std::vector<std::string> userList_;

        char valid_sha[SHA_LEN];
        bool debug_;
        static constexpr char chal_chars[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    };

    

}