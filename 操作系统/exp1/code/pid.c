#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int main() {
	pid_t pid, pid1;

	// fork a child process
	pid = fork();

	// error occurred
	if (pid < 0) {
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}
	// child process
	else if (pid == 0) {
		pid1 = getpid();
		printf("child: pid = %d\n", pid);
		printf("child: pid1 = %d\n", pid1);
	}
	// parent process
	else {
		//wait(NULL);
		pid1 = getpid();
		printf("parent: pid = %d\n", pid);
		printf("parent: pid1 = %d\n", pid1);
		wait(NULL);
	}

	return 0;
}