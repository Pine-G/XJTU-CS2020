//�Թ�����2.0

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define WALL 1				//ǽ
#define ROUTE 0			    //·
#define PATH 2				//�Թ�ͨ��
#define OFFSET 5			//���ƫ����

//�����Ͻ�Ϊԭ�㣬��ֱ����Ϊx��������ˮƽ����Ϊy��������

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
		int size;			//�Թ���С
		do {
			system("cls");
			printf("----------------�Թ���Ϸ----------------\n\n");
			printf("�������Թ��Ĵ�С��");
			scanf("%d", &size);
			if (size < 3) {
				printf("�Թ���С������ڵ���3��");
				Sleep(800);
			}
		} while (size < 3);

		int **maze = (int **)malloc((size + 4) * sizeof(int *));
		for (int i = 0; i < size + 4; i++)
			maze[i] = (int *)malloc((size + 4) * sizeof(int));
		createMaze(maze, size);

		printf("��ʼ��Ϸ��\n\n");
		printMaze(maze, size);
		char ch = playGame(maze, size);

		if (ch == 'q') {
			if (mazePath(maze, size)) {
				SetCCPos(OFFSET, 0);
				printMaze(maze, size);
			} else {
				SetCCPos(size + 4 + OFFSET, 0);
				puts("�Թ�������ͨ·��");
			}
		}

		for (int i = 0; i < size + 4; i++)
			free(maze[i]);		//�ͷ��ڴ�
		free(maze);

		printf("�Ƿ�ʼ��һ����Ϸ��(Y)�� (N)��");
		fflush(stdin);			//������뻺����
		choice = getchar();
		if (choice == 'y')
			choice += 'A' - 'a';
	} while (choice == 'Y');

	return 0;
}

//�����Թ�
void createMaze(int **maze, int size) {
	srand((unsigned)time(NULL));	//�������������
	//��ʼʱ���Թ�ȫΪǽ
	for (int i = 0; i < size + 4; i++)
		for (int j = 0; j < size + 4; j++)
			maze[i][j] = WALL;
	//�߽籣������ֹ�Թ����ڴ�
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
		//��ֹ�ڴ��γɻ�
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

//�����Թ�
position nextPos(position cur, int direction) {
	position next;
	switch (direction) {
		case 0:
			next.x = cur.x + 1;				//��������
			next.y = cur.y;
			break;
		case 1:
			next.x = cur.x;					//��������
			next.y = cur.y + 1;
			break;
		case 2:
			next.x = cur.x - 1;				//��������
			next.y = cur.y ;
			break;
		case 3:
			next.x = cur.x;					//��������
			next.y = cur.y - 1;
			break;
	}
	return next;
}

//�����ƶ�
position move(position player, int direction) {
	SetCCPos(player.x + OFFSET, player.y * 2);
	printf("  ");
	player = nextPos(player, direction);
	SetCCPos(player.x + OFFSET, player.y * 2);
	printf("��");
	return player;
}

//��ʼ��Ϸ
char playGame(int **maze, int size) {
	HideCursor();				//���ؿ���̨���
	position player = {2, 2};
	SetCCPos(player.x + OFFSET, player.y * 2);
	printf("��");
	char ch;

	while ((ch = getch()) != 'q') {
		switch (ch) {
			case 'a':			//����
				if (maze[player.x ][player.y - 1] != WALL)
					player = move(player, 3);
				break;
			case 's':			//����
				if (maze[player.x + 1][player.y ] != WALL)
					player = move(player, 0);
				break;
			case 'd':			//����
				if (maze[player.x ][player.y + 1] != WALL)
					player = move(player, 1);
				break;
			case 'w':			//����
				if (maze[player.x - 1][player.y ] != WALL)
					player = move(player, 2);
				break;
			default:
				continue;
		}
		if (player.x == size + 1 && player.y == size + 1) {
			SetCCPos(size + 4 + OFFSET, 0);
			puts("�ɹ���");
			break;
		}
	}
	return ch;
}

//����Թ�����
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

//����Թ�·��
void printMaze(int **maze, int size) {
	for (int i = 0; i < size + 4; i++) {
		for (int j = 0; j < size + 4; j++) {
			SetConsoleOutputCP(437);		//���ô���ҳ�������չASCII���ַ���
			if (i == 2 && j == 1) {
				SetConsoleOutputCP(936);	//���ô���ҳ��������������ַ���
				printf("��");
				continue;
			}
			if (i == size + 1 && j == size + 2) {
				SetConsoleOutputCP(936);
				printf("��");
				continue;
			}
			if (maze[i][j] == WALL)
				putchar(254);		//��ɫ�����ַ���2���ֽ�
			else if (maze[i][j] == ROUTE)
				printf("  ");
			else
				putchar(234);		//���ַ���2���ֽ�
		}
		putchar('\n');
	}
	SetConsoleOutputCP(936);
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

//���ù��λ��
void SetCCPos(int x, int y) {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);		//��ȡ��ע������
	COORD pos;
	pos.X = y;
	pos.Y = x;
	SetConsoleCursorPosition(hOut, pos);		//ƫ�ƹ��λ��
}

//���ع��
void HideCursor(void) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);	//��ȡ����̨�����Ϣ
	CursorInfo.bVisible = false;				//���ؿ���̨���
	SetConsoleCursorInfo(handle, &CursorInfo);	//���ÿ���̨���״̬
}