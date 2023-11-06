#pragma once
extern "C" {
   #include "cfe.h"
}
namespace moonlighter::io
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t TIME_LEN = 32;
    static const size_t MAX_REQUESTS = 4;
    static const size_t MAX_DL_BUFFERS = 10;
    static const size_t MAX_IMG_NAME_SIZE = 32;
    static const size_t MAX_GPS_RESPONSE_SIZE = 1024;
    static const size_t MAX_GPS_CMD_SIZE = 256;
    static const size_t CAMERA_POWER = 4;
    static const size_t GPS_POWER = 5;
    static const size_t DEFAULT_EXPOSURE_US = 250;
    // data structure
    struct OS_PACK Exposure
    {
        uint32_t microsecs;
    };
    struct OS_PACK TimeData
    {
        uint8_t yr;
        uint8_t mo;
        uint8_t day;
        uint8_t hr;
        uint8_t min;
        uint8_t sec;
        uint8_t csec;
    };
    struct OS_PACK FileRequest
    {
        char path[64];
        uint32_t start;
        uint32_t nBytes;
    };
    struct OS_PACK CmdAck
    {
        uint16_t msgid;
        uint16_t size;
        uint8_t status;
        uint32_t accepted;
        uint32_t rejected;
        uint8_t power_state;
        uint32_t tlm[MAX_DL_BUFFERS];
    };
    struct OS_PACK DownlinkFileData
    {
        char filename[32];
        uint32_t start;
        uint32_t nBytes;
        uint32_t filesize;
        uint32_t crc32;
    };
    struct OS_PACK FileSize
    {
        char filename[32];
        uint32_t filesize;
    };
    struct OS_PACK TmFileStat
    {
        uint8_t bufferid;
        char files[32];
    };
    struct OS_PACK FilterRequest
    {
        uint16_t msgid;
    };
    struct OS_PACK FilterSettings
    {
        uint16_t msgid;
        uint8_t allowed;
    };
    struct OS_PACK HouseKeeping
    {
        uint32_t upPktAccepted;
        uint32_t upPktRejected;
        uint64_t upBytes;
        uint64_t downBytes;
        uint16_t downlinkChunkSize;
        uint16_t waitNackHighDelay;
        uint8_t waitNackHighState;
    };
    struct OS_PACK AcbPoint
    {
        float qAxis[3];
        float qScalar;
    };
    struct OS_PACK AcbMode
    {
        uint8_t mode;
        uint8_t minutes;
    };
    struct OS_PACK AcbTlmRate
    {
        uint16_t tlm_ms;
    };
    struct OS_PACK AcbState
    {
        TimeData utc;
        uint8_t mode;
        float position[3];
        float velocity[3];
        float qAxis[3];
        float qScalar;
        float w[3];
        float star_tacker_delay;
    };
    struct OS_PACK AcbRaw
    {
        TimeData utc;
        uint8_t mode;
        uint8_t submode;
        float qAxis[3];
        float qScalar;
        float w[3];
        uint16_t starTrackerDelayMs;
        uint16_t rwSpeedRps[3];
        int8_t mtbState[3];
        float positionEci[3];
        float velocityEci[3];
    };
    struct OS_PACK NovatelCommand
    {
        int16_t id;
        uint8_t type;
        char command[MAX_GPS_CMD_SIZE];
    };
    struct OS_PACK NovatelResponse
    {
        int16_t id;
        char response[MAX_GPS_RESPONSE_SIZE];
        char crc[8];
    };
    struct OS_PACK Picture
    {
        char img[MAX_IMG_NAME_SIZE];
    };
    struct OS_PACK PictureDownload
    {
        char img[MAX_IMG_NAME_SIZE];
        char download_directory[MAX_IMG_NAME_SIZE];
    };
    struct OS_PACK CameraFile
    {
        uint16_t item;
        uint16_t max_items;
        char searchpath[50];
        char path[50];
    };
    struct OS_PACK ImageState
    {
        char img[50];
        uint8_t state;
    };
    struct OS_PACK Power
    {
        uint8_t device;
        uint8_t power;
    };
    struct OS_PACK Polling
    {
        uint8_t allowed;
    };
    struct OS_PACK StrTime
    {
        char time[TIME_LEN];
    };
    struct OS_PACK IntTime
    {
        uint32_t seconds;
        uint32_t microseconds;
    };
    struct OS_PACK UpdateIoConfig
    {
        uint16_t chunkSize;
        uint16_t waitNackHighDelay;
        uint8_t waitNackHighFlag;
        uint16_t muxDelay;
    };
}