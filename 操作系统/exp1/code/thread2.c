#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

void *runner(void *param);

int main() {
	int info;
	pthread_t tid;

	printf("Parent: syscall(SYS_gettid) = %ld, getpid() = %d, pthread_self() = %lu\n", syscall(SYS_gettid), getpid(),
	       pthread_self());

	// create the thread
	info = pthread_create(&tid, NULL, runner, NULL);
	if (info != 0) {
		printf("Thread create failed!\n");
		return -1;
	}

	// wait for the thread to exit
	pthread_join(tid, NULL);

	return 0;
}

void *runner(void *param) {
	printf("Child: syscall(SYS_gettid) = %ld, getpid() = %d, pthread_self() = %lu\n", syscall(SYS_gettid), getpid(),
	       pthread_self());
	//system("./hello2");
	execl("./hello2", "hello2", NULL);
}