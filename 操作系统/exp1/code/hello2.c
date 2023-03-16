//线程中system或execl函数调用的代码，可执行文件名为hello2
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

int main() {
    printf("Hello World!\n");
    printf("hello: syscall(SYS_gettid) = %ld, getpid() = %d, pthread_self() = %lu\n", syscall(SYS_gettid), getpid(), pthread_self());

    return 0;
}
