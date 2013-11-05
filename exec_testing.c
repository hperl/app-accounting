#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
    char bin[]   = "/bin/echo";
    char file[]  = "echo";
    char *args[] = {"echo", "test", 0};
    char **envp  = NULL;
    pid_t pid;

    puts("execve");
    pid = fork();
    (pid == 0) && execve(bin, args, envp);
    waitpid(pid, NULL, 0);

    puts("execl");
    pid = fork();
    (pid == 0) && execl(bin, args[0], args[1], (char*)NULL);
    waitpid(pid, NULL, 0);

    puts("execle");
    pid = fork();
    (pid == 0) && execle(bin, args[0], args[1], (char*)NULL, envp);
    waitpid(pid, NULL, 0);

    puts("execlp");
    pid = fork();
    (pid == 0) && execlp(file, args[0], args[1], (char*)NULL, envp);
    waitpid(pid, NULL, 0);

    puts("execv");
    pid = fork();
    (pid == 0) && execv(bin, args);
    waitpid(pid, NULL, 0);

    puts("execvp");
    pid = fork();
    (pid == 0) && execvp(file, args);
    waitpid(pid, NULL, 0);
}

