#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int count = 0;

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
        count--;
        printf("child: count = %d\n", count);
        printf("child: count address = %p\n", &count);
    }
    // parent process
    else {
        wait(NULL);
        count++;
        printf("parent: count = %d\n", count);
        printf("parent: count address = %p\n", &count);
    }

    count++;
    printf("before return: count = %d\n", count);
    printf("before return: count address = %p\n", &count);

    return 0;
}
