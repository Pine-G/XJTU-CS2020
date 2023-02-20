//�Թ�����
//0��ʾ���ϰ���1��ʾ���ϰ�

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define WALL 1				//ǽ
#define PASSABLE 0			//��·��ͨ
#define PATH 2				//�Թ�ͨ��
#define ROW 8				//�Թ�����
#define COL 9				//�Թ�����

//�Թ�
int maze[ROW + 2][COL + 2] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1},
	{1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1},
	{1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1},
	{1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
	{1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1},
	{1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
	{1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

typedef struct {
	int x;					//λ������
	int y;
} position;

typedef struct {
	position pos;			//λ�ü���������
	int direction;
} possibleWay;

typedef struct Node {
	possibleWay data;		//ջ��Ԫ��
	struct Node *next;
} node;

node *top = NULL;			//ջ��ָ��

//����ԭ��
void ClearStack(void);
bool StackIsEmpty(void);
bool StackIsFull(void);
bool push(possibleWay data);
possibleWay pop(void);

position nextPos(position cur, int direction);
bool mazePath(void);
void printPath(void);

int main(void) {
	printf("�������Թ����£�\n");
	printPath();
	if (mazePath()) {
		printf("�Թ�����ͨ·:\n");
		printPath();
	} else
		printf("�Թ�������ͨ·��\n");

	return 0;
}

//�����Թ�
position nextPos(position cur, int direction) {
	position next;
	switch (direction) {
		case 0:
			next.x = cur.x;				//��������
			next.y = cur.y + 1;
			break;
		case 1:
			next.x = cur.x + 1;			//��������
			next.y = cur.y;
			break;
		case 2:
			next.x = cur.x;				//��������
			next.y = cur.y - 1;
			break;
		case 3:
			next.x = cur.x - 1;			//��������
			next.y = cur.y;
			break;
	}
	return next;
}

//����Թ�����
bool mazePath(void) {
	position cur;
	possibleWay way;
	cur.x = 1;
	cur.y = 1;
	do {
		if (maze[cur.x][cur.y] == PASSABLE) {
			maze[cur.x][cur.y] = PATH;
			way.pos = cur;
			way.direction = 0;
			push(way);
			if (cur.x == ROW && cur.y == COL) {
				ClearStack();
				return true;
			}
			cur = nextPos(cur, 0);
		} else {
			way = pop();
			while (way.direction == 3 && !StackIsEmpty()) {
				maze[way.pos.x][way.pos.y] = 0;
				way = pop();
			}
			if (way.direction < 3) {
				way.direction++;
				push(way);
				cur = nextPos(way.pos, way.direction);
			}
		}
	} while (!StackIsEmpty());
	return false;
}

//����Թ�·��
void printPath(void) {
	for (int i = 0; i < ROW + 2; i++) {
		for (int j = 0; j < COL + 2; j++) {
			if (maze[i][j] == WALL)
				printf("@ ");
			else if (maze[i][j] == PASSABLE)
				printf("  ");
			else
				printf("* ");
		}
		putchar('\n');
	}
}

//����ջ
void ClearStack(void) {
	while (top) {
		node *temp = top;
		top = top->next;
		free(temp);
	}
}

//�ж�ջ�Ƿ�Ϊ��
bool StackIsEmpty(void) {
	return (top == NULL) ? true : false;
}

//�ж�ջ�Ƿ�����
bool StackIsFull(void) {
	node *temp = (node *)malloc(sizeof(node));
	if (temp) {
		free(temp);
		return false;
	} else
		return true;
}

//��ջ
bool push(possibleWay data) {
	if (StackIsFull())
		return false;
	node *temp = (node *)malloc(sizeof(node));
	temp->data = data;
	temp->next = top;
	top = temp;
	return true;
}

//��ջ
possibleWay pop(void) {
	if (StackIsEmpty()) {
		printf("False! Stack is Empty.\n");
		exit(0);
	} else {
		possibleWay data = top->data;
		node *temp = top;
		top = top->next;
		free(temp);
		return data;
	}
}