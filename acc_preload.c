#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <zmq.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "symbols.h"

void
do_nothing(int sig) {}

void
app_acc_send_msg(char *buf, size_t size)
{
    zmq_msg_t msg;
    void *sock, *context;
    pid_t pid;

    /* fork again */
    if ((pid = fork()) == -1) {
        perror("fork");
    }
    if (pid > 0) {
        /* parent exits */
        return;
    }
    /* child: detatch from terminal */
    if (setsid() == -1) {
        perror("setsid");
    }
    /* Redirect standard files to /dev/null */
    chdir("/");
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    /* send with zeromq */
    context = zmq_init(1);
    sock = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sock, "tcp://localhost:5555");
    zmq_msg_init_data(&msg, buf, size, NULL, NULL);
    zmq_send(sock, &msg, 0);
    zmq_close(sock);
    zmq_term(context);
}

int
execve(const char *path, char *const argv[], char *const envp[])
{
    pid_t pid;

    /* suppress INT and TERM signals => wrappers never die :)
     * Note: child loses those after the syscall           */
    signal(SIGINT,  do_nothing);
    signal(SIGTERM, do_nothing);
    /* fork: parent watches, child plays */
    if ((pid = fork()) == -1) {
        perror("fork in execve");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        /* go play */
        return syscall(SYS_execve, path, argv, envp);
    } else {
        const size_t BUF_MAX = 500;
        time_t start_time, end_time;
        int wait_status;
        char buf[BUF_MAX];
        size_t pos = 0;

        signal(SIGINT,  do_nothing);
        signal(SIGTERM, do_nothing);
        /* go watch */
        start_time = time(NULL);
        /* wait on process until it terminated (stops are already ignored) */
        waitpid(pid, &wait_status, 0);
        end_time = time(NULL);
        /* log info */
        pos += snprintf(buf+pos, BUF_MAX-pos, "%s: \"%s\"\n%s:\n",
                        KEY_BINARY_PATH, path,
                        KEY_EXIT_INFO);
        if (pos >= BUF_MAX) { goto buffer_overflow; }
        if (WIFEXITED(wait_status)) {
            pos += snprintf(buf+pos, BUF_MAX-pos, "  %s: true\n  %s: %i\n",
                            KEY_EXIT_NORMAL,
                            KEY_EXIT_SIGNAL, WEXITSTATUS(wait_status));
            if (pos >= BUF_MAX) { goto buffer_overflow; }
        } else {
            pos += snprintf(buf+pos, BUF_MAX-pos, "  %s: false\n  %s: %i\n  %s: \"%s\"\n",
                            KEY_EXIT_NORMAL,
                            KEY_EXIT_SIGNAL, WTERMSIG(wait_status),
                            KEY_EXIT_SIGNAL_STR, strsignal(WTERMSIG(wait_status)));
            if (pos >= BUF_MAX) { goto buffer_overflow; }
        }
        pos += snprintf(buf+pos, BUF_MAX-pos,
                        "%s:\n  %s: %i\n  %s: %i\n%s: %s%s: %s",
                        KEY_USER_INFO,
                        KEY_UID, getuid(),
                        KEY_GID, getgid(),
                        KEY_TIME_START, asctime(localtime(&start_time)),
                        KEY_TIME_END,   asctime(localtime(&end_time)));
        if (pos > BUF_MAX) { goto buffer_overflow; }

    buffer_overflow:
        app_acc_send_msg(buf, pos+1);
    }
    exit(EXIT_SUCCESS);
}

