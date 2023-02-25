//迷宫问题
//0表示无障碍，1表示有障碍

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define WALL 1				//墙
#define PASSABLE 0			//此路可通
#define PATH 2				//迷宫通道
#define ROW 8				//迷宫行数
#define COL 9				//迷宫列数

//迷宫
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
	int x;				//位置坐标
	int y;
} position;

typedef struct {
	position pos;			//位置及搜索方向
	int direction;
} possibleWay;

typedef struct Node {
	possibleWay data;		//栈中元素
	struct Node *next;
} node;

node *top = NULL;			//栈顶指针

//函数原型
void ClearStack(void);
bool StackIsEmpty(void);
bool StackIsFull(void);
bool push(possibleWay data);
possibleWay pop(void);

position nextPos(position cur, int direction);
bool mazePath(void);
void printPath(void);

int main(void) {
	printf("创建的迷宫如下：\n");
	printPath();
	if (mazePath()) {
		printf("迷宫存在通路:\n");
		printPath();
	} else
		printf("迷宫不存在通路！\n");

	return 0;
}

//搜索迷宫
position nextPos(position cur, int direction) {
	position next;
	switch (direction) {
		case 0:
			next.x = cur.x;			//向下搜索
			next.y = cur.y + 1;
			break;
		case 1:
			next.x = cur.x + 1;			//向右搜索
			next.y = cur.y;
			break;
		case 2:
			next.x = cur.x;			//向上搜索
			next.y = cur.y - 1;
			break;
		case 3:
			next.x = cur.x - 1;			//向左搜索
			next.y = cur.y;
			break;
	}
	return next;
}

//求解迷宫问题
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

//输出迷宫路径
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

//销毁栈
void ClearStack(void) {
	while (top) {
		node *temp = top;
		top = top->next;
		free(temp);
	}
}

//判断栈是否为空
bool StackIsEmpty(void) {
	return (top == NULL) ? true : false;
}

//判断栈是否已满
bool StackIsFull(void) {
	node *temp = (node *)malloc(sizeof(node));
	if (temp) {
		free(temp);
		return false;
	} else
		return true;
}

//入栈
bool push(possibleWay data) {
	if (StackIsFull())
		return false;
	node *temp = (node *)malloc(sizeof(node));
	temp->data = data;
	temp->next = top;
	top = temp;
	return true;
}

//出栈
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