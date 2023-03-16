#include <stdio.h>
#include <pthread.h>

int count = 0;
pthread_mutex_t mutex;
void *add(void *param);
void *sub(void *param);

int main() {
	int info;
	pthread_t tid1, tid2;

	// create the mutex lock
	pthread_mutex_init(&mutex, NULL);

	// create the thread
	info = pthread_create(&tid1, NULL, add, NULL);
	if (info != 0) {
		printf("Thread1 create failed!\n");
		return -1;
	}
	info = pthread_create(&tid2, NULL, sub, NULL);
	if (info != 0) {
		printf("Thread2 create failed!\n");
		return -1;
	}

	// wait for the thread to exit
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	printf("count = %d\n", count);

	return 0;
}

void *add(void *param) {
	pthread_mutex_lock(&mutex);

	// critical section
	for (int i = 0; i < 5000; i++)
		count++;
	printf("thread1: count = %d\n", count);

	pthread_mutex_unlock(&mutex);
}

void *sub(void *param) {
	pthread_mutex_lock(&mutex);

	// critical section
	for (int i = 0; i < 5000; i++)
		count--;
	printf("thread2: count = %d\n", count);

	pthread_mutex_unlock(&mutex);
}