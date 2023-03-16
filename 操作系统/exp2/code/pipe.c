#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>

int pid1, pid2;

int main() {
	int fd[2];
	char InPipe[5000];		//�����������
	char c1 = '1', c2 = '2';
	pipe(fd);				//�����ܵ�

	while ((pid1 = fork()) == -1);

	if (pid1 == 0) {
		//�ӽ���1
		lockf(fd[1], 1, 0);		//�����ܵ�
		for (int i = 0; i < 2000; i++)
			write(fd[1], &c1, 1);
		sleep(5);
		lockf(fd[1], 0, 0);		//�������
		exit(0);
	} else {

		while ((pid2 = fork()) == -1);

		if (pid2 == 0) {
			//�ӽ���2
			lockf(fd[1], 1, 0);		//�����ܵ�
			for (int i = 0; i < 2000; i++)
				write(fd[1], &c2, 1);
			sleep(5);
			lockf(fd[1], 0, 0);		//�������
			exit(0);
		} else {
			//������
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