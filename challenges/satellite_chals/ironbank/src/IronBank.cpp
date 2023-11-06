#include "IronBank.hpp"
#include "ib_messages.hpp"
#include "clocks.hpp"
#include "crypto.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std::chrono_literals;
namespace cromulence::ironbank
{

    
    std::string join( std::vector<std::string> vec , std::string sep )
    {
        std::string out = "";
        for( size_t k=0; k < vec.size() ; k++  )
        {
            out += vec[k];
            if( k != (vec.size()-1))
            {
                out+=",";
            }
        }
        return out;
    }
    Vault::Vault( std::string secrets , std::string users   ) :
     keepGoing_( true )
     ,challengeIssued_(false)
     ,debug_(false)

    {

        udp_.reply_mode();
        // VULN: EXPOSE this udp port in the binarys
        udp_.connect( PORT , 5s );
        // load secrets file
        std::ifstream f( secrets.c_str() );
        json data =  json::parse(f);
        uploadPath_ = data["script_path"];
        downloadPath_ = data["download_path"];
        exePath_ = data["run_path"];
        key_ = data["key"];

        // load users
        std::ifstream userFile( users.c_str() );
        std::string line;
        while (std::getline(userFile , line))
        {
            userList_.push_back( line ) ;
        }
        memset( valid_sha , 0  , SHA_LEN);
    }
    Vault::~Vault( )
    {
        udp_.disconnect();
    }

    void Vault::execute() 
    {
        keepGoing_ = true;
        while( true == keepGoing_ )
        {
            //printf("yolonoscope\n");
            try
            {
                this->handle_commands();
            }
            catch(...)
            {

            }
        }
    }
    void Vault::stop( )
    {
        keepGoing_ = false;
    }

    void Vault::clear()
    {
        challengeIssued_ =  false;
    }

    bool Vault::user_exists( std::string in )
    {
        for( size_t k=0; k < userList_.size(); k++ )
        {
            if(  ( in.compare( userList_[k]) ) == 0 )
            {
                return true;
            }
            
        }
        return false;
    }

    void Vault::generate_key()
    {
        // generate the next challenge
        std::string tmp_s;
        tmp_s.reserve(CHALLENGE_LEN);

        for (size_t i = 0; i < CHALLENGE_LEN; i++)
        {
            int idx = rand() % (sizeof( chal_chars )-1);
            tmp_s += chal_chars[idx];
        }
    
        challenge_ = tmp_s;
        // generate the next key
        
        cromulence::crypto::MD5 md5;

        md5.update( challenge_.c_str() , challenge_.size() );
        md5.update( key_.c_str() , key_.size() ) ;
        unsigned char* out;
        out = md5.digest();
        memcpy( valid_sha , out , SHA_LEN);
        // leave this test sha commented out - its only for testing
        //std::string test = "abcdefghijklmnop";
        //memcpy( valid_sha , test.c_str() , SHA_LEN);
    }
    size_t  Vault::burn( size_t n )
    {
        size_t sum =0;
        for(size_t k=0;k<n;k++)
        {
        }
        return sum;
    }
    bool Vault::validate_key( const command &cmd)
    {
        //size_t TEST_LEN = SHA_LEN;
        size_t TEST_LEN = 10; //Note: We thought in testing that 16 bytes was really really hard on orbit, so we reduced to 10
        for( size_t k = 0 ; k < TEST_LEN ; k++  )
        {
                            

            if( cmd.auth[k] != valid_sha[k] )
            {

                return false;
            }
            burn(1250);
            // VULN Added: This is an obvious vuln in the RE but it makes the challenge reliable in solve. This is hard enough without the RE being obtuse
        }
        return true;
    }
    bool Vault::is_protected( const command &cmd )
    {
        return cmd.id >PROTECTED;
    }
    void Vault::issue_challenge( const command &cmd )
    { 
        //printf("Issuing new challenge\n");
        generate_key();
        user_ = std::string( cmd.data );
        response_ = challenge_;
        challengeIssued_ = true;
        //printf("%s\n", valid_sha );
    }
    void Vault::print_sha( char * in )
    {
        for( size_t k = 0 ; k < SHA_LEN ; k++)
        {
            uint8_t *x = reinterpret_cast<uint8_t*>( in + k );
            printf("%02x", *x );
        }
        printf("\n");
    }
    void Vault::handle_commands()
    {
        bool ok( false) ;
        command cmd;
        response_ = "ok";
        ok = udp_.read(cmd  );
        if( false == ok )
        {
            return;
        }
        uint64_t before,after;

        before = ticks();
        if(  true == is_protected( cmd) )
        {
            if( true == challengeIssued_ )
            {
                // requires key
                bool pass(false);
                //printf("Validating:\n\n" );
                //print_sha( cmd.auth );
                //print_sha( valid_sha );
                pass = validate_key( cmd );
                if( true == pass )
                {   
                    // do the command
                    protected_command( cmd );
                    challengeIssued_ = false;
                }
                else
                {
                    // VULN ADDED: In reality on a bad auth it should throw away the challenge and demand that you start over
                    response_ = "bad auth";
                }
            }
            else
            {
                response_ = "request auth first";
            }

        }
        else
        {
            unprotected_command( cmd );
        }
        after = ticks();
        if( true == debug_ )
        {
            uint64_t delta = after - before;
            response_ += " took: ";
            response_ += std::to_string( delta );
        }
        //printf("Ironbank Response: %s\n", response_.c_str() );
        udp_.sends( response_ );

    }

    void Vault::protected_command(const  command &cmd )
    {
        switch( cmd.id )
        {
        case LOAD_SCRIPT:
            // grab a script file
            load_script(  cmd);
            break;
        case DOWNLOAD_SCRIPT:
            // move script to download folder
            download_script( cmd );
            break;
        case DOWNLOAD_FLAG:
            // move flag to download folder
            download_flag();
            break;
        default:
            break;
        }
    }
    void Vault::unprotected_command( const command &cmd )
    {
        switch( cmd.id )
        {
        case AUTH:
            issue_challenge( cmd ); 
            break;
        case LIST_USERS:
            list_users();
            break;
        case ENABLE_DEBUG:
            printf("Debug on");
            debug_ = true;
            response_ = "ok";
            break;
        case DISABLE_DEBUG:
            debug_ = false;
            response_ = "ok";
            break;
        default:
            break;

        }
    }
    void Vault::download_flag()
    {
        
        std::string flagname;
        std::string flagPath;
        printf("User %s got the flag\n", user_.c_str() );
        if( true == user_exists( user_ ) )
        {
            FILE* fp;
            flagPath = downloadPath_ + "/" + user_ + ".ib_flag";
            fp = fopen( flagPath.c_str(), "wt");
            if( nullptr == fp )
            {
                printf("Bad path saving to tmp\n");
                flagPath = "/tmp/" + user_ + ".ib_flag";
                fp = fopen( flagPath.c_str() , "wt");
                if( nullptr == fp )
                {
                    printf("extra bad path what the deuce\n");
                    return;
                }
            }
            fprintf( fp , "%s\n", user_.c_str() );
            // create a signed flag 
            cromulence::crypto::MD5 sign;
            sign.update( user_.c_str(), user_.size() );
            sign.update( key_.c_str(), key_.size()  );
            uint8_t* hash = reinterpret_cast<uint8_t*>( sign.digest() );
            for( size_t k = 0; k < sign.hash_size() ; k++ )
            {
                fprintf( fp, "%02x",  *( hash+k) );
                printf("%02x",  *( hash+k));
            }
            fprintf(fp,"\n");
            printf("\n");
            fclose( fp );
            response_ = "ok";
        }
    }

    void Vault::load_script( const command &cmd  )
    {
        std::string newScriptPath;
        std::string storedScriptPath;
        std::string scriptName;
        //printf("Moving user %s\n" , user_.c_str() );
        if( true == user_exists( user_ ) )
        {
            uint8_t id;
            id = *reinterpret_cast<const uint8_t*>( cmd.data );
            clear_run_folder();
            scriptName =  user_ + "_" + std::to_string( id ) + ".lua";
            
            newScriptPath = exePath_ + "/" + scriptName;
            storedScriptPath = uploadPath_ + "/" + scriptName;

            printf("Copy %s --> %s \n" ,storedScriptPath.c_str(), newScriptPath.c_str() );

            std::filesystem::copy( storedScriptPath, newScriptPath );
            std::filesystem::permissions( newScriptPath,
                                          std::filesystem::perms::owner_all | 
                                          std::filesystem::perms::group_all | 
                                          std::filesystem::perms::others_all,
                                          std::filesystem::perm_options::replace);
            response_ = "ok";
        }
        

    }
    void Vault::download_script(  const command &cmd)
    {
        std::string newScriptPath;
        std::string storedScriptPath;
        std::string scriptName;
        if( true == user_exists( user_ ) )
        {
            
            uint8_t id;
            id = *reinterpret_cast<const uint8_t*>( cmd.data );
            scriptName =  user_ + "_" + std::to_string( id ) + ".lua";
            newScriptPath = downloadPath_ + "/" + scriptName;
            storedScriptPath = uploadPath_ + "/" + scriptName;
            std::filesystem::copy( storedScriptPath, newScriptPath );
            response_ = "ok";
        }
    }
    void Vault::list_users( )
    {

        std::string userlist;
        userlist = join( userList_ , ",");
        response_ = userlist;
    }

    void Vault::clear_run_folder()
    {
        for (const auto& entry : std::filesystem::directory_iterator(exePath_))
        {
            std::filesystem::remove_all(entry.path());
        }
        
    }
}