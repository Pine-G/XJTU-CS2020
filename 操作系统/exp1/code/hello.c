//�ӽ�����system��execl�������õĴ��룬��ִ���ļ���Ϊhello
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("Hello World!\n");
	printf("hello: pid = %d\n", getpid());

	return 0;
}