//农夫过河问题的求解
//0表示在南岸，1表示在北岸
//从左到右依次表示农夫、狼、菜、羊

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define SIZE 16

int visited[SIZE];			//标志位（用于标志结点是否被访问过）
int stack[SIZE];			//栈
int top = 0;				//栈顶指针
int number = 0;				//方案数目
int interface_bottom = 0;	//使程序退出时返回信息在界面底端

char *situation[SIZE] = {"农夫 狼 菜 羊", "农夫 狼 菜", "农夫 狼    羊", "农夫 狼",
                         "农夫    菜 羊", "农夫    菜", "农夫       羊", "农夫",
                         "     狼 菜 羊", "     狼 菜", "     狼    羊", "     狼",
                         "        菜 羊", "        菜", "           羊", "     空"
                        };

//函数原型
void CreateGraph(int matrix[][SIZE]);

int farmer_location(int state);
int wolf_location(int state);
int cabbage_location(int state);
int sheep_location(int state);
int unsafe(int state);

int case1(int i, int j);
int case2(int i, int j);
int case3(int i, int j);
int unavailable(int i, int j);

void DFS(int matrix[][SIZE], int start, int end);

int StackIsEmpty(void);
int StackIsFull(void);
int push(int data);
int pop(void);

void PrintPath(void);

void SetCCPos(int x, int y);

int main(void) {
	int matrix[SIZE][SIZE];		//邻接矩阵存储图
	CreateGraph(matrix);		//创建图
	DFS(matrix, 0, 15);			//深度优先搜索

	return 0;
}

//创建图
void CreateGraph(int matrix[][SIZE]) {
	//初始化邻接矩阵
	//对角线元素为0，其余都为1
	for (int i = 0; i < SIZE; i++) {
		visited[i] = 0;	//将标志位初始为0
		for (int j = i; j < SIZE; j++) {
			if (i == j)
				matrix[i][j] = 0;
			else			//邻接矩阵的对称性
				matrix[i][j] = matrix[j][i] = 1;
		}
	}

	//判断不安全状态（狼和羊、羊和菜）
	//使得其它顶点与不安全状态的顶点间无路径
	for (int i = 0; i < SIZE; i++) {
		if (unsafe(i)) {
			visited[i] = 1;		//访问了不安全状态的顶点
			for (int j = 0; j < SIZE; j++)
				matrix[i][j] = matrix[j][i] = 0;
		}
	}

	//判断其它顶点是否相连
	//即从某一顶点出发能否直接到达另一顶点
	for (int i = 0; i < SIZE - 1; i++)
		for (int j = i + 1; j < SIZE ; j++)
			if (visited[i] == 0 && visited[j] == 0 && unavailable(i, j))
				matrix[i][j] = matrix[j][i] = 0;

	//将标志位重置为0
	for (int i = 0; i < SIZE; i++)
		visited[i] = 0;
}

//判断农夫、狼、菜、羊的位置
int farmer_location(int state) {
	return ((state & 0x08) != 0);
}

int wolf_location(int state) {
	return ((state & 0x04) != 0);
}

int cabbage_location(int state) {
	return ((state & 0x02) != 0);
}

int sheep_location(int state) {
	return ((state & 0x01) != 0);
}

//判断不安全状态
int unsafe(int state) {
	int judge = 0;
	//狼和羊单独在一起
	if (wolf_location(state) == sheep_location(state) && wolf_location(state) != farmer_location(state))
		judge = 1;
	//羊和菜单独在一起
	if (cabbage_location(state) == sheep_location(state) && sheep_location(state) != farmer_location(state))
		judge = 1;
	return judge;
}

//狼到达对岸
int case1(int i, int j) {
	if (wolf_location(i) != wolf_location(j)) {
		if (sheep_location(i) == sheep_location(j) && cabbage_location(i) == cabbage_location(j))
			return 1;
		else
			return 0;
	} else
		return 0;
}

//菜到达对岸
int case2(int i, int j) {
	if (cabbage_location(i) != cabbage_location(j)) {
		if (sheep_location(i) == sheep_location(j) && wolf_location(i) == wolf_location(j))
			return 1;
		else
			return 0;
	} else
		return 0;
}

//羊到达对岸
int case3(int i, int j) {
	if (sheep_location(i) != sheep_location(j)) {
		if (wolf_location(i) == wolf_location(j) && cabbage_location(i) == cabbage_location(j))
			return 1;
		else
			return 0;
	} else
		return 0;
}

//判断其它顶点是否相连
int unavailable(int i, int j) {
	int judge = 1;
	// <i,j>边存在
	if (farmer_location(i) != farmer_location(j)) {
		//农夫独自返回对岸
		if ((i & 0x07) == (j & 0x07))
			judge = 0;
		//农夫带东西返回对岸
		if (case1(i, j) || case2(i, j) || case3(i, j))
			judge = 0;
	}
	return judge;
}

//深度优先搜索
void DFS(int matrix[][SIZE], int start, int end) {
	visited[start] = 1;
	push(start);
	if (start == end)
		PrintPath();
	for (int j = 0; j < SIZE; j++)
		if (!visited[j] && matrix[start][j] == 1)
			DFS(matrix, j, end);
	pop();
	visited[start] = 0;
}

//输出路径
void PrintPath(void) {
	int i;
	int row = 10;
	int col = 30;
	SetCCPos(col * number, row * i);
	printf("方案%d：", number + 1);
	for (i = 0; i < top - 1; i++) {
		SetCCPos(col * number, row * i + 2);
		puts(situation[stack[i]]);
		SetCCPos(col * number, row * i + 3);
		puts("--------------");
		SetCCPos(col * number, row * i + 4);
		puts("    小河");
		SetCCPos(col * number, row * i + 5);
		puts("--------------");
		SetCCPos(col * number, row * i + 6);
		int opposite = (~(stack[i])) & 0x0F;
		puts(situation[opposite]);
		SetCCPos(col * number, row * i + 7);
		SetCCPos(col * number, row * i + 8);
		SetCCPos(col * number, row * i + 9);
		puts("     ->");
		SetCCPos(col * number, row * i + 10);
		SetCCPos(col * number, row * i + 11);
	}
	SetCCPos(col * number, row * i + 2);
	puts(situation[stack[i]]);
	SetCCPos(col * number, row * i + 3);
	puts("--------------");
	SetCCPos(col * number, row * i + 4);
	puts("    小河");
	SetCCPos(col * number, row * i + 5);
	puts("--------------");
	SetCCPos(col * number, row * i + 6);
	int opposite = (~(stack[i])) & 0x0F;
	puts(situation[opposite]);
	interface_bottom = ((row * (top - 1) + 7) > interface_bottom) ? (row * (top - 1) + 7) : interface_bottom;
	SetCCPos(0, interface_bottom);
	number++;
}

//判断栈是否为空
int StackIsEmpty(void) {
	return (top == 0) ? 1 : 0;
}

//判断栈是否已满
int StackIsFull(void) {
	return (top == SIZE) ? 1 : 0;
}

//入栈
int push(int data) {
	if (StackIsFull())
		return 0;
	else
		stack[top++] = data;
	return 1;
}

//出栈
int pop(void) {
	if (StackIsEmpty()) {
		printf("False! Stack is Empty.\n");
		exit(0);
	} else
		return stack[--top];
}

//设置光标位置
void SetCCPos(int x, int y) {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);//获取标注输出句柄
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hOut, pos);//偏移光标位置
}
