#include "subprocess.hpp"
#include <sys/param.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <paths.h>
#include <filesystem>
#include <grp.h>
#include <vector>
#include <thread>
namespace cromulence
{

    SubProcess::SubProcess() : open_(false),
                               pipe_(nullptr)
    {
        lastStatus_ = NOT_STARTED;
    }

    SubProcess::~SubProcess()
    {
        stop();
    }

    void SubProcess::run(std::string cmd, std::vector<std::string> args, std::vector<std::string> env)
    {
        // build the args list
        lastStatus_ = NOT_STARTED;
        std::vector<char *> argv;
        argv.push_back(const_cast<char *>(cmd.c_str())); // Push the command as the first argument

        for (const auto &arg : args)
        {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }
        argv.push_back(NULL);
        // build the env list
        std::vector<char*> envs;
        for( const auto &en : env )
        {
            envs.push_back( const_cast<char*>(en.c_str()));
        }
        envs.push_back(NULL);
        // get absolute path
        std::string absolute_cmd;
        absolute_cmd = std::filesystem::absolute(cmd);
        // dont zombie stuff please
        if (true == this->alive())
        {
            return;
        }
        int stdin_pipe[2], stdout_pipe[2];
        SPC_PIPE *p;
        if (!(p = (SPC_PIPE *)malloc(sizeof(SPC_PIPE))))
        {
            pipe_ = 0;
            return;
        }
        p->read_fd = p->write_fd = 0;
        p->child_pid = -1;
        if (pipe(stdin_pipe) == -1)
        {
            free(p);
            pipe_ = 0;
            return;
        }
        if (pipe(stdout_pipe) == -1)
        {
            close(stdin_pipe[1]);
            close(stdin_pipe[0]);
            free(p);
            pipe_ = 0;
            return;
        }
        if (!(p->read_fd = fdopen(stdout_pipe[0], "r")))
        {
            close(stdout_pipe[1]);
            close(stdout_pipe[0]);
            close(stdin_pipe[1]);
            close(stdin_pipe[0]);
            free(p);
            pipe_ = 0;
            return;
        }
        if (!(p->write_fd = fdopen(stdin_pipe[1], "w")))
        {
            fclose(p->read_fd);
            close(stdout_pipe[1]);
            close(stdin_pipe[1]);
            close(stdin_pipe[0]);
            free(p);
            pipe_ = 0;
            return;
        }
        if ((p->child_pid = this->spc_fork()) == -1)
        {
            fclose(p->write_fd);
            fclose(p->read_fd);
            close(stdout_pipe[1]);
            close(stdin_pipe[0]);
            free(p);
            pipe_ = 0;
            return;
        }
        if (!p->child_pid)
        {
            /* this is the child process */
            close(stdout_pipe[0]);
            close(stdin_pipe[1]);
            if (stdin_pipe[0] != 0)
            {
                dup2(stdin_pipe[0], 0);
                close(stdin_pipe[0]);
            }
            if (stdout_pipe[1] != 1)
            {
                dup2(stdout_pipe[1], 1);
                close(stdout_pipe[1]);
            }

            // char *argv[] = {NULL};
            printf("Starting: %s\n", absolute_cmd.c_str());
            int o;
            o = execve(absolute_cmd.c_str(), argv.data(), envs.data());
            printf("%d\n", o);
            exit(127);
        }
        close(stdout_pipe[1]);
        close(stdin_pipe[0]);
        pipe_ = p;
        open_ = true;
        lastStatus_ = RUNNING;
        return;
    }
    int SubProcess::stop()
    {
        int status;
        pid_t pid = -1;
        if (true == open_)
        {
            if (pipe_->child_pid != -1)
            {
                // Kill nicely
                if(kill(pipe_->child_pid, SIGTERM) == -1){
                    // Try killing with style
                    kill(pipe_->child_pid, SIGKILL);
                    // If this doesn't kill it may already be dead Jim
                }
                // Try and get the exit code 5 times without blocking
                for(size_t i=0; i < 5; i++){
                    pid = waitpid(pipe_->child_pid, &status, WNOHANG);
                    if(pid == -1 && errno == EINTR){
                        continue;
                    }
                    else{
                        break;
                    }
                }
            }
            if (pipe_->read_fd)
                fclose(pipe_->read_fd);
            if (pipe_->write_fd)
                fclose(pipe_->write_fd);
            free(pipe_);
            open_ = false;
            if (pid != -1 && WIFEXITED(status))
                return WEXITSTATUS(status);
            else
                return (pid == -1 ? -1 : 0);
        }
        return -1;
    }
    bool SubProcess::alive()
    {
        if (pipe_ == 0)
        {
            return false;
        }
        int status;
        pid_t result = waitpid(pipe_->child_pid, &status, WNOHANG);

        //int out;
        //out = kill(pipe_->child_pid, 0);
        if( result == 0 )
        {
            lastStatus_ = RUNNING;
        }
        else
                {
            lastStatus_ = EXITED;
        }
        return lastStatus_ == RUNNING;
    }
    
    bool SubProcess::wait_for_end( std::chrono::milliseconds total , std::chrono::milliseconds step )
    {
        size_t retryLimit = total / step;
        size_t count(0);
        bool wait;
        bool succeed( true );
        wait = alive();
        while( true == wait  )
        {
            std::this_thread::sleep_for( step );
            if( count > retryLimit )
            {
                wait = false;
                succeed = (false == alive() );
            }
            else
            {
                wait = alive();
                count++;
            }
        }
        return succeed;
    }
    SubProcess::Status SubProcess::status()
    {
        return lastStatus_;
    }
    pid_t SubProcess::spc_fork()
    {
        pid_t childpid;

        if ((childpid = fork()) == -1)
            return -1;

        /* Reseed PRNGs in both the parent and the child */
        /* See Chapter 11 for examples */

        /* If this is the parent process, there's nothing more to do */
        if (childpid != 0)
            return childpid;

        /* This is the child process */
        spc_sanitize_files();   /* Close all open files.  See Recipe 1.1 */
        spc_drop_privileges(1); /* Permanently drop privileges.  See Recipe 1.3 */

        return 0;
    }
    void SubProcess::spc_drop_privileges(int permanent)
    {
        gid_t newgid = getgid(), oldgid = getegid();
        uid_t newuid = getuid(), olduid = geteuid();

        if (!permanent)
        {
            /* Save information about the privileges that are being dropped so that they
             * can be restored later.
             */
            orig_gid = oldgid;
            orig_uid = olduid;
            orig_ngroups = getgroups(NGROUPS_MAX, orig_groups);
        }

        /* If root privileges are to be dropped, be sure to pare down the ancillary
         * groups for the process before doing anything else because the setgroups(  )
         * system call requires root privileges.  Drop ancillary groups regardless of
         * whether privileges are being dropped temporarily or permanently.
         */
        if (!olduid)
            setgroups(1, &newgid);

        if (newgid != oldgid)
        {

            if (setregid((permanent ? newgid : -1), newgid) == -1)
                abort();
        }

        if (newuid != olduid)
        {

            if (setreuid((permanent ? newuid : -1), newuid) == -1)
                abort();
        }

        /* verify that the changes were successful */
        if (permanent)
        {
            if (newgid != oldgid && (setegid(oldgid) != -1 || getegid() != newgid))
                abort();
            if (newuid != olduid && (seteuid(olduid) != -1 || geteuid() != newuid))
                abort();
        }
        else
        {
            if (newgid != oldgid && getegid() != newgid)
                abort();
            if (newuid != olduid && geteuid() != newuid)
                abort();
        }
    }
    void SubProcess::spc_sanitize_files(void)
    {
        int fd, fds;
        struct stat st;

        /* Make sure all open descriptors other than the standard ones are closed */
        if ((fds = getdtablesize()) == -1)
            fds = OPEN_MAX;
        for (fd = 3; fd < fds; fd++)
            close(fd);

        /* Verify that the standard descriptors are open.  If they're not, attempt to
         * open them using /dev/null.  If any are unsuccessful, abort.
         */
        for (fd = 0; fd < 3; fd++)
            if (fstat(fd, &st) == -1 && (errno != EBADF || !open_devnull(fd)))
                abort();
    }
    void SubProcess::spc_restore_privileges(void)
    {
        if (geteuid() != orig_uid)
            if (seteuid(orig_uid) == -1 || geteuid() != orig_uid)
                abort();
        if (getegid() != orig_gid)
            if (setegid(orig_gid) == -1 || getegid() != orig_gid)
                abort();
        if (!orig_uid)
            setgroups(orig_ngroups, orig_groups);
    }
    int SubProcess::open_devnull(int fd)
    {
        FILE *f = 0;

        if (!fd)
            f = freopen(_PATH_DEVNULL, "rb", stdin);
        else if (fd == 1)
            f = freopen(_PATH_DEVNULL, "wb", stdout);
        else if (fd == 2)
            f = freopen(_PATH_DEVNULL, "wb", stderr);
        return (f && fileno(f) == fd);
    }
    /*
     * Kills all processes whose exec file points to a specific path.
     * Returns: the number of procs killed
     * If number returned == 0 then nothing was killed. If return == 100, most likely an error
     * as this is the highest number of processes of the same executable it will kill
     * 
     * Minimum value of max_count is 1
    */
    size_t SubProcess::killZombiesByExecPath(const std::filesystem::path& executablePath, size_t max_count){
        pid_t pid;
        size_t count = 0;

        if(max_count == 0)
            max_count = 1;

        while(count < max_count && (pid = findProcessByExecutablePath(executablePath)) > 0){
            kill(pid, SIGTERM);
            count++;
        }

        return count;
    }
    /*
     * Finds the process using a specific executable file and returns its pid
     * returns a negative number if not found
    */
    int SubProcess::findProcessByExecutablePath(const std::filesystem::path& executablePath) {
        std::filesystem::path normalized_path = normalizePath(executablePath);
        const std::filesystem::directory_iterator end;
        
        for (std::filesystem::directory_iterator iter("/proc"); iter != end; ++iter) {
            if (!iter->is_directory()) {
                continue;
            }

            // Extract the PID from the directory name. 
            // If the directory name is not a number (i.e., not a PID), skip this directory.
            int pid = -1;
            try {
                pid = std::stoi(iter->path().filename());
            } catch (const std::invalid_argument& ex) {
                continue;
            }
            
            // Construct the path to the 'exe' symlink for this process.
            std::filesystem::path symlinkPath = *iter;
            symlinkPath /= "exe";
            
            // Check if the symlink exists and is accessible.
            try{
                if (!std::filesystem::exists(symlinkPath)) {
                    continue;
                }
            } catch (const std::filesystem::filesystem_error& ex){
                // Failed to access file"
                continue; 
            }


            // Read the target of the symlink.
            // This operation can fail if the process has exited after we checked if the symlink exists,
            // or if we don't have permission to read the symlink.
            std::filesystem::path targetPath;
            try {
                targetPath = std::filesystem::read_symlink(symlinkPath);
            } catch (const std::filesystem::filesystem_error& ex) {
                // Failed to read symlink
                continue;
            }

            // If the target of the symlink matches the executablePath (normalized), print the PID.
            if (targetPath == normalized_path) {
                // Found pid!
                return pid;
            }
        }
        
        return -1;
    }

    // Normalizes a path to remove reiterative junk
    std::filesystem::path SubProcess::normalizePath(const std::filesystem::path& messyPath) {
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(messyPath).make_preferred();

        return canonicalPath;
    }
}