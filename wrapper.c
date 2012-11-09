#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <zmq.h>


#include "symbols.h"

const size_t BUF_MAX = 500;

/* Returns the real path of the executable */
char *
get_realpath(char *basename)
{
    FILE *fp;
    char *realpath;
    char *DIRS[] = { "/bin", "/sbin", NULL };
    char *dir;
    size_t i = 0;
    
    while ((dir = DIRS[i++])) {
        realpath = malloc(strlen(dir) + strlen(basename) + 2); /* 2 = '/' + \0 */
        sprintf(realpath, "%s/%s", dir, basename);
        /* check if file exists */
        fp = fopen(realpath, "r");
        if (fp) {
            /* File exists, we are done :) */
            fclose(fp);
            return realpath;
        }
        /* Continue searching ... */
        free(realpath);
    }
    /* All paths searched, no file existed -> abort */
    i = 0;
    printf("ERROR: `%s' not found in directories ", basename);
    while ((dir = DIRS[i++])) {
        printf("%s", dir);
        DIRS[i] ? printf(", ") : printf("!");
    }
    exit(EXIT_FAILURE);
}

void
do_nothing(int sig)
{
    return;
}

void
send(char *buf, size_t size)
{
    zmq_msg_t msg;
    void *sock, *context;
    
    context = zmq_init(1);
    sock = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sock, "tcp://localhost:5555");
    zmq_msg_init_data(&msg, buf, size, NULL, NULL);
    zmq_send(sock, &msg, 0);
    zmq_close(sock);
    zmq_term(context);
}

int
main(int argc, char *argv[])
{
    char *bin, *realpath;
    pid_t pid;
    time_t start_time, end_time;
    int wait_status;
    char buf[BUF_MAX];
    size_t pos = 0;
        
    /* suppress INT and TERM signals */
    signal(SIGINT,  do_nothing);
    signal(SIGTERM, do_nothing);
    bin = basename(argv[0]);
    realpath = get_realpath("ls");
    /* fork and execute binary in subprocess */
    pid = fork();
    if (pid == 0) {
        /* subprocess: execute binary */
        if (execve(realpath, argv, NULL) == -1) {
            perror(realpath);
            exit(EXIT_FAILURE);
        }
        /* does not return */
    } else if (pid > 0) {
        /* parent process: monitor */
        start_time = time(NULL);
        /* wait on process until it terminated (stops are already ignored) */
        waitpid(pid, &wait_status, 0);
        end_time = time(NULL);
        /* log info */        
        pos += snprintf(buf+pos, BUF_MAX-pos, "%s: \"%s\"\n%s:\n",
                        KEY_BINARY_PATH, realpath,
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
        send(buf, pos+1);
    } else {
        /* fork() == -1  ->  error! */
        perror("fork");
        exit(EXIT_FAILURE);
    }
    /* All is good now */
    exit(EXIT_SUCCESS);
    
buffer_overflow:
    printf("ERROR: Buffer too small.");
    exit(EXIT_FAILURE);
}


