#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>

pid_t pid1, pid2;

void parent() {
	kill(pid1, 16);
	kill(pid2, 17);
}

void child1() {
	printf("Child process 1 is killed by parent !!\n");
	exit(0);
}

void child2() {
	printf("Child process 2 is killed by parent !!\n");
	exit(0);
}

int main() {

	while ((pid1 = fork()) == -1);

	if (pid1 > 0) {
		while ((pid2 = fork()) == -1);

		if (pid2 > 0) {
			//父进程
			signal(SIGQUIT, parent);
			signal(SIGALRM, parent);
			alarm(5);
			//signal(SIGINT, parent);
			wait(NULL);
			wait(NULL);
			printf("Parent process is killed !!\n");
			exit(0);
		} else {
			//子进程2
			signal(17, child2);
			signal(SIGQUIT, SIG_IGN);
			//signal(SIGINT, SIG_IGN);
			pause();
		}
	} else {
		//子进程1
		signal(16, child1);
		signal(SIGQUIT, SIG_IGN);
		//signal(SIGINT, SIG_IGN);
		pause();
	}
}