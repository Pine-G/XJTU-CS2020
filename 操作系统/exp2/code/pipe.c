#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>

int pid1, pid2;

int main() {
	int fd[2];
	char InPipe[5000];		//定义读缓冲区
	char c1 = '1', c2 = '2';
	pipe(fd);				//创建管道

	while ((pid1 = fork()) == -1);

	if (pid1 == 0) {
		//子进程1
		lockf(fd[1], 1, 0);		//锁定管道
		for (int i = 0; i < 2000; i++)
			write(fd[1], &c1, 1);
		sleep(5);
		lockf(fd[1], 0, 0);		//解除锁定
		exit(0);
	} else {

		while ((pid2 = fork()) == -1);

		if (pid2 == 0) {
			//子进程2
			lockf(fd[1], 1, 0);		//锁定管道
			for (int i = 0; i < 2000; i++)
				write(fd[1], &c2, 1);
			sleep(5);
			lockf(fd[1], 0, 0);		//解除锁定
			exit(0);
		} else {
			//父进程
			wait(NULL);
			wait(NULL);
			read(fd[0], InPipe, 4000);
			InPipe[4000] = '\0';
			printf("%s\n", InPipe);
			exit(0);
		}
	}

	return 0;
}