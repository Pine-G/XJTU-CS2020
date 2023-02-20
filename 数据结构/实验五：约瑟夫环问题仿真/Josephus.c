//约瑟夫环问题仿真
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
	unsigned int number;		//编号
	unsigned int password;		//密码
	struct Node *next;			//指向下一个结点的指针
} node;

//构建具有n个结点的循环链表
node *CreateList(int n) {
	node *head = (node *)malloc(sizeof(node));
	printf("请输入第1个人的密码：");
	scanf("%d", &head->password);
	head->number = 1;
	head->next = head;
	node *temp = head;
	for (int i = 2; i <= n; i++) {
		temp->next = (node *)malloc(sizeof(node));
		printf("请输入第%d个人的密码：", i);
		scanf("%d", &temp->next->password);
		temp->next->number = i;
		temp->next->next = head;
		temp = temp->next;
	}
	return head;
}

//打印 p->next 的编号，并返回其密码
int OutList(node **Head, node *p) {
	int num, password;
	//删除首元结点
	if (p->next == (*Head)) {
		num = (*Head)->number;
		password = (*Head)->password;
		(*Head) = (*Head)->next;
		free(p->next);
		p->next = (*Head);
	}

	//删除其它结点
	else {
		node *temp = p->next;
		num = temp->number;
		password = temp->password;
		p->next = temp->next;
		free(temp);
	}
	printf("%d ", num);
	return password;
}

//仿真
void simulation(node *head, int n, int m) {
	int state = 1;		//用于标志第一次报数
	node *p;
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < m - 1; i++) {
			if (state) {
				p = head;
				state = 0;
			} else
				p = p->next;
		}
		m = OutList(&head, p);
	}
}

int main(void) {
	int N, m;
	printf("请输入总人数：");
	scanf("%d", &N);
	printf("请输入初始报数上限：");
	scanf("%d", &m);
	node *head = CreateList(N);
	printf("依次出列的人的编号为：\n");
	simulation(head, N, m);

	return 0;
}