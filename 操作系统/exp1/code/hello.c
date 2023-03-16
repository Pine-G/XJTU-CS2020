//子进程中system或execl函数调用的代码，可执行文件名为hello
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("Hello World!\n");
	printf("hello: pid = %d\n", getpid());

	return 0;
}