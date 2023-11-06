#pragma once
#include <memory>
#include <cstring>
extern "C" {
   #include "cfe.h"
}
namespace cromulence::ironbank
{

enum Command
{
    UNPROTECTED=0,
    LIST_USERS,
    AUTH,
    ENABLE_DEBUG,
    DISABLE_DEBUG,
    PROTECTED,
    LOAD_SCRIPT,
    DOWNLOAD_SCRIPT,
    DOWNLOAD_FLAG
};
static const size_t CHALLENGE_LEN=64;
static const size_t SHA_LEN=16;
static const size_t DATA_LEN=16;
static const size_t USER_LIST_SIZE= DATA_LEN*5;

static const uint16_t PORT = 12123;
struct OS_PACK command
{
    char auth[SHA_LEN];
    uint8_t id;
    char data[DATA_LEN];

    command()
    {
        memset( auth, 0 ,SHA_LEN);
        id = 0;
        memset( data, 0 ,DATA_LEN);
    }
};

struct OS_PACK user_list
{
    uint8_t id;
};


}