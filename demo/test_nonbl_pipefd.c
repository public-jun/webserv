#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
** to test how non-blocking pipe fd for read behaves ...
**   - before data is written
**   - between first data and second
**   - after write fd has closed
**   - after child process closed
**
** ※よい子はprintfの排他処理をしましょう
*/

int main(void) {
    pid_t   pid;
    int     pipe_fd[2];
    char    buf[1024];
    ssize_t ret;
    int     cnt;

    // create pipe
    if (pipe(pipe_fd) == -1) {
        printf("[parent] failed to create pipe\n");
        return (1);
    }

    // create process
    pid = fork();
    if (pid == -1) {
        printf("[parent] failed to create child process\n");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return (1);
    }

    // child process
    if (pid == 0) {
        close(pipe_fd[0]); // not used in child process

        printf("[child] sleep 3 sec.\n");
        sleep(3);

        printf("[child] writing to pipe.\n");
        write(pipe_fd[1], "Calling from child process...", 29);

        printf("[child] sleep 3 sec.\n");
        sleep(3);

        printf("[child] writing to pipe.\n");
        write(pipe_fd[1], "2nd Calling from child process...", 33);

        // comment out following 2 lines to check
        // the case process exits without close pipefd
        printf("[child] close pipe.\n");
        close(pipe_fd[1]);

        printf("[child] sleep 3 sec.\n");
        sleep(3);

        printf("[child] exit.\n");
        exit(1);
    }

    // parent process
    close(pipe_fd[1]); // not used in parent process
    // fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);

    cnt = 12;
    while (cnt-- > 0) {
        printf("[parent] read from child via nonblocking pipe fd\n");
        errno = 0; // reset errno to check result of read
        ret   = read(pipe_fd[0], buf, sizeof(buf) - 1);
        printf("    ret = %zd, errno = %d\n", ret, errno);
        if (ret > 0) {
            buf[ret] = '\0';
            printf("    data = \"%s\"\n", buf);
        }

        // check if child process finished if not yet
        if (pid != 0) {
            printf("[parent] check child process finished\n");
            if (waitpid(pid, NULL, WNOHANG) == pid) {
                printf("[parent] catch child process\n");
                pid = 0; // means that child process already finished
            }
        }

        // repeat with 1sec interaval
        sleep(1);
    }
    return (0);
}