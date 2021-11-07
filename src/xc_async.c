#include <xc_async.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdint.h>


int async_exec(async_info_t *async, char *path, char * const argv[])
{
        int filedes[2];
        int filedes_err[2];
        pid_t pid;

        if (pipe(filedes) == -1) {
                return ASYNC_PIPE;
        }

        if (pipe(filedes_err) == -1) {
                return ASYNC_PIPE;
        }

        pid = fork();
        if (pid == -1) {
                return ASYNC_NOFORK;
        }

        if (pid != 0) {
                /* here we are in the parent process */
                async->std_out = filedes[0];
                async->err_out = filedes_err[0];
                /* parent does not write into pipe */
                close(filedes[1]);
                close(filedes_err[1]);
                return ASYNC_OK;
        }

        /* here we are in the child process */

        /* STDOUT is closed if open and reopened as referece to
         * filedes[1] atomically */
        while((dup2(filedes[1], STDOUT_FILENO) == -1)
              && (errno == EINTR)) {}

        while((dup2(filedes_err[1], STDERR_FILENO) == -1)
              && (errno == EINTR)) {}

        /* child process does not write to filedes[1] directly */
        close(filedes[1]);
        close(filedes_err[1]);

        /* filedes[0] is the pipe output, only used by parent process */
        close(filedes[0]);
        close(filedes_err[0]);

        execv(path, argv);
        /* execl replaces this process, so we can only get here
         * if execl failed */
        return ASYNC_EXECV;
}

