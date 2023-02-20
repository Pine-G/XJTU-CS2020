//Լɪ���������
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
	unsigned int number;		//���
	unsigned int password;		//����
	struct Node *next;			//ָ����һ������ָ��
} node;

//��������n������ѭ������
node *CreateList(int n) {
	node *head = (node *)malloc(sizeof(node));
	printf("�������1���˵����룺");
	scanf("%d", &head->password);
	head->number = 1;
	head->next = head;
	node *temp = head;
	for (int i = 2; i <= n; i++) {
		temp->next = (node *)malloc(sizeof(node));
		printf("�������%d���˵����룺", i);
		scanf("%d", &temp->next->password);
		temp->next->number = i;
		temp->next->next = head;
		temp = temp->next;
	}
	return head;
}

//��ӡ p->next �ı�ţ�������������
int OutList(node **Head, node *p) {
	int num, password;
	//ɾ����Ԫ���
	if (p->next == (*Head)) {
		num = (*Head)->number;
		password = (*Head)->password;
		(*Head) = (*Head)->next;
		free(p->next);
		p->next = (*Head);
	}

	//ɾ���������
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

//����
void simulation(node *head, int n, int m) {
	int state = 1;		//���ڱ�־��һ�α���
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
	printf("��������������");
	scanf("%d", &N);
	printf("�������ʼ�������ޣ�");
	scanf("%d", &m);
	node *head = CreateList(N);
	printf("���γ��е��˵ı��Ϊ��\n");
	simulation(head, N, m);

	return 0;
}