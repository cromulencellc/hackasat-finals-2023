#pragma once
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <filesystem>
namespace cromulence
{
    class SubProcess
    {
        typedef struct {
            FILE  *read_fd;
            FILE  *write_fd;
            pid_t child_pid;

        } SPC_PIPE;
    public:
    
        enum Status
        {
            NOT_STARTED = 0,
            RUNNING,
            EXITED,
            FAILED,
            INTERRUPTED
        };
        SubProcess( );
        ~SubProcess();

        static std::filesystem::path normalizePath(const std::filesystem::path& messyPath);
        static size_t killZombiesByExecPath(const std::filesystem::path& executablePath, size_t max_count=100);
        void run( std::string cmd,  std::vector<std::string> args , std::vector<std::string> env={});
        int stop( );
        bool alive();
        bool wait_for_end( std::chrono::milliseconds total , std::chrono::milliseconds step );
        Status status();
    protected:
        pid_t spc_fork();
        void spc_sanitize_files();
        void spc_drop_privileges(int permanent);
        void spc_restore_privileges();
        int open_devnull(int fd);
        static pid_t findProcessByExecutablePath(const std::filesystem::path& executablePath);

        bool open_;
        SPC_PIPE *pipe_;
        Status lastStatus_;
        int   orig_ngroups = -1;
        gid_t orig_gid = -1;
        uid_t orig_uid = -1;
        gid_t orig_groups[NGROUPS_MAX];
    

        static const unsigned int OPEN_MAX=256;
    };
}