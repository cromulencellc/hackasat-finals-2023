#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::staging
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t QUEUE_NAME_LENGTH = 32;
    static const size_t FILE_NAME_LENGTH = 64;
    static const size_t MAX_USERS = 5;
    static const size_t MAX_ITEMS = 10;
    // data structure
    struct OS_PACK QueueId
    {
        char queue[QUEUE_NAME_LENGTH];
    };
    struct OS_PACK RecoverFile
    {
        char name[FILE_NAME_LENGTH];
    };
    struct OS_PACK QueueSize
    {
        char queue[QUEUE_NAME_LENGTH];
        uint8_t max_entries;
    };
    struct OS_PACK StagingSize
    {
        uint8_t max_images;
    };
    struct OS_PACK TeamImage
    {
        char team[QUEUE_NAME_LENGTH];
        char img[QUEUE_NAME_LENGTH];
    };
    struct OS_PACK Image
    {
        char img[QUEUE_NAME_LENGTH];
    };
    struct OS_PACK ItemStatus
    {
        char img[QUEUE_NAME_LENGTH];
        uint8_t status;
    };
    struct OS_PACK QueueName
    {
        char name[QUEUE_NAME_LENGTH];
    };
    struct OS_PACK QueueListing
    {
        QueueName queues[MAX_USERS];
    };
    struct OS_PACK QueueStatus
    {
        char name[QUEUE_NAME_LENGTH];
        ItemStatus item[MAX_ITEMS];
    };
}