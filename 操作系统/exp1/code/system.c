#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main() {
    // fork a child process
    pid_t pid = fork();

    // error occurred
    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    // child process
    else if (pid == 0) {
        printf("child: pid = %d\n", getpid());
        //system("./hello");
        execl("./hello", "hello", NULL);
    }
    // parent process
    else {
        wait(NULL);
        printf("parent: pid = %d\n", getpid());
    }

    return 0;
}
