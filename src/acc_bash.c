#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>


/* OS X has somehow different names for ptrace macros: */
#ifndef PTRACE_TRACEME
#define PTRACE_TRACEME PT_TRACE_ME
#endif

int
main(int argc, char *argv[])
{
    pid_t child;
    
    puts(">>> Starting bash");
    if ((child = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child == 0) { /* in child */
//        if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
//            perror("ptrace");
//            exit(EXIT_FAILURE);
//        }
        execve("/bin/bash", NULL, NULL);
    } else { /* in parent */
        int s;
        
        while (waitpid(child, &s, 0) != -1) {
            if (WIFSIGNALED(s)) {
                printf(">>> Child signaled %s", strsignal(WTERMSIG(s)));
            }
            if (WIFSTOPPED(s)) {
                printf(">>> Child signaled %s", strsignal(WSTOPSIG(s)));
            }
        }
        if (errno == EINTR) {
            puts(">>> Bash exited");
            exit(EXIT_SUCCESS);

        }
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
        
}