//迷宫问题2.0

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define WALL 1				//墙
#define ROUTE 0			    //路
#define PATH 2				//迷宫通道
#define OFFSET 5			//光标偏移量

//以左上角为原点，竖直向下为x轴正方向，水平向右为y轴正方向

typedef struct {
	int x;					//位置坐标
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

void SetCCPos(int x, int y);
void HideCursor(void);

void dig(int **maze, int x, int y);
void createMaze(int **maze, int size);

position move(position player, int direction);
char playGame(int **maze, int size);
position nextPos(position cur, int direction);
bool mazePath(int **maze, int size);
void printMaze(int **maze, int size);

int main(void) {
	char choice;
	do {
		int size;			//迷宫大小
		do {
			system("cls");
			printf("----------------迷宫游戏----------------\n\n");
			printf("请输入迷宫的大小：");
			scanf("%d", &size);
			if (size < 3) {
				printf("迷宫大小必须大于等于3！");
				Sleep(800);
			}
		} while (size < 3);

		int **maze = (int **)malloc((size + 4) * sizeof(int *));
		for (int i = 0; i < size + 4; i++)
			maze[i] = (int *)malloc((size + 4) * sizeof(int));
		createMaze(maze, size);

		printf("开始游戏：\n\n");
		printMaze(maze, size);
		char ch = playGame(maze, size);

		if (ch == 'q') {
			if (mazePath(maze, size)) {
				SetCCPos(OFFSET, 0);
				printMaze(maze, size);
			} else {
				SetCCPos(size + 4 + OFFSET, 0);
				puts("迷宫不存在通路！");
			}
		}

		for (int i = 0; i < size + 4; i++)
			free(maze[i]);		//释放内存
		free(maze);

		printf("是否开始新一轮游戏？(Y)是 (N)否：");
		fflush(stdin);			//清空输入缓冲区
		choice = getchar();
		if (choice == 'y')
			choice += 'A' - 'a';
	} while (choice == 'Y');

	return 0;
}

//创建迷宫
void createMaze(int **maze, int size) {
	srand((unsigned)time(NULL));	//设置随机数种子
	//开始时，迷宫全为墙
	for (int i = 0; i < size + 4; i++)
		for (int j = 0; j < size + 4; j++)
			maze[i][j] = WALL;
	//边界保护，防止迷宫被挖穿
	for (int i = 0; i < size + 4; i++) {
		maze[0][i] = ROUTE;
		maze[i][0] = ROUTE;
		maze[size + 3][i] = ROUTE;
		maze[i][size + 3] = ROUTE;
	}
	dig(maze, 2, 2);
	maze[size + 1][size + 1] = ROUTE;
}

void dig(int **maze, int x, int y) {
	if (maze[x][y] == WALL) {
		//防止挖穿形成环
		if (maze[x + 1][y] + maze[x - 1][y] + maze[x][y + 1] + maze[x][y - 1] >= 3) {
			maze[x][y] = ROUTE;
			int directions[4] = {0, 1, 2, 3};
			for (int i = 4; i > 0; i--) {
				int r = rand() % i;
				int temp = directions[r];
				directions[r] = directions[i - 1];
				directions[i - 1] = temp;
				switch (directions[i - 1]) {
					case 0:
						dig(maze, x - 1, y);
						break;
					case 1:
						dig(maze, x, y + 1);
						break;
					case 2:
						dig(maze, x + 1, y);
						break;
					case 3:
						dig(maze, x, y - 1);
						break;
					default:
						break;
				}
			}
		}
	}
}

//搜索迷宫
position nextPos(position cur, int direction) {
	position next;
	switch (direction) {
		case 0:
			next.x = cur.x + 1;				//向下搜索
			next.y = cur.y;
			break;
		case 1:
			next.x = cur.x;					//向右搜索
			next.y = cur.y + 1;
			break;
		case 2:
			next.x = cur.x - 1;				//向上搜索
			next.y = cur.y ;
			break;
		case 3:
			next.x = cur.x;					//向左搜索
			next.y = cur.y - 1;
			break;
	}
	return next;
}

//人物移动
position move(position player, int direction) {
	SetCCPos(player.x + OFFSET, player.y * 2);
	printf("  ");
	player = nextPos(player, direction);
	SetCCPos(player.x + OFFSET, player.y * 2);
	printf("人");
	return player;
}

//开始游戏
char playGame(int **maze, int size) {
	HideCursor();				//隐藏控制台光标
	position player = {2, 2};
	SetCCPos(player.x + OFFSET, player.y * 2);
	printf("人");
	char ch;

	while ((ch = getch()) != 'q') {
		switch (ch) {
			case 'a':			//左移
				if (maze[player.x ][player.y - 1] != WALL)
					player = move(player, 3);
				break;
			case 's':			//下移
				if (maze[player.x + 1][player.y ] != WALL)
					player = move(player, 0);
				break;
			case 'd':			//右移
				if (maze[player.x ][player.y + 1] != WALL)
					player = move(player, 1);
				break;
			case 'w':			//上移
				if (maze[player.x - 1][player.y ] != WALL)
					player = move(player, 2);
				break;
			default:
				continue;
		}
		if (player.x == size + 1 && player.y == size + 1) {
			SetCCPos(size + 4 + OFFSET, 0);
			puts("成功！");
			break;
		}
	}
	return ch;
}

//求解迷宫问题
bool mazePath(int **maze, int size) {
	position cur = {2, 2};
	possibleWay way;

	do {
		if (maze[cur.x][cur.y] == ROUTE) {
			maze[cur.x][cur.y] = PATH;
			way.pos = cur;
			way.direction = 0;
			push(way);
			if (cur.x == size + 1 && cur.y == size + 1) {
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
void printMaze(int **maze, int size) {
	for (int i = 0; i < size + 4; i++) {
		for (int j = 0; j < size + 4; j++) {
			SetConsoleOutputCP(437);		//设置代码页，输出扩展ASCII码字符集
			if (i == 2 && j == 1) {
				SetConsoleOutputCP(936);	//设置代码页，输出简体中文字符集
				printf("入");
				continue;
			}
			if (i == size + 1 && j == size + 2) {
				SetConsoleOutputCP(936);
				printf("出");
				continue;
			}
			if (maze[i][j] == WALL)
				putchar(254);		//白色方块字符，2个字节
			else if (maze[i][j] == ROUTE)
				printf("  ");
			else
				putchar(234);		//Ω字符，2个字节
		}
		putchar('\n');
	}
	SetConsoleOutputCP(936);
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
	if (StackIsEmpty())
		return;
	else {
		possibleWay data = top->data;
		node *temp = top;
		top = top->next;
		free(temp);
		return data;
	}
}

//设置光标位置
void SetCCPos(int x, int y) {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);		//获取标注输出句柄
	COORD pos;
	pos.X = y;
	pos.Y = x;
	SetConsoleCursorPosition(hOut, pos);		//偏移光标位置
}

//隐藏光标
void HideCursor(void) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);	//获取控制台光标信息
	CursorInfo.bVisible = false;				//隐藏控制台光标
	SetConsoleCursorInfo(handle, &CursorInfo);	//设置控制台光标状态
}