//ũ�������������
//0��ʾ���ϰ���1��ʾ�ڱ���
//���������α�ʾũ���ǡ��ˡ���
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define SIZE 16

int visited[SIZE];		//��־λ�����ڱ�־����Ƿ񱻷��ʹ���
int stack[SIZE];		//ջ
int top = 0;			//ջ��ָ��
int number = 0;			//������Ŀ
int interface_bottom = 0;		//ʹ�����˳�ʱ������Ϣ�ڽ���׶�

char *situation[SIZE] = {"ũ�� �� �� ��", "ũ�� �� ��", "ũ�� ��    ��", "ũ�� ��",
                         "ũ��    �� ��", "ũ��    ��", "ũ��       ��", "ũ��",
                         "     �� �� ��", "     �� ��", "     ��    ��", "     ��",
                         "        �� ��", "        ��", "           ��", "     ��"
                        };

//����ԭ��
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
	int matrix[SIZE][SIZE];			//�ڽӾ���洢ͼ
	CreateGraph(matrix);			//����ͼ
	DFS(matrix, 0, 15);				//�����������

	return 0;
}

//����ͼ
void CreateGraph(int matrix[][SIZE]) {
	//��ʼ���ڽӾ���
	//�Խ���Ԫ��Ϊ0�����඼Ϊ1
	for (int i = 0; i < SIZE; i++) {
		visited[i] = 0;		//����־λ��ʼΪ0
		for (int j = i; j < SIZE; j++) {
			if (i == j)
				matrix[i][j] = 0;
			else			//�ڽӾ���ĶԳ���
				matrix[i][j] = matrix[j][i] = 1;
		}
	}

	//�жϲ���ȫ״̬���Ǻ�����Ͳˣ�
	//ʹ�����������벻��ȫ״̬�Ķ������·��
	for (int i = 0; i < SIZE; i++) {
		if (unsafe(i)) {
			visited[i] = 1;		//�����˲���ȫ״̬�Ķ���
			for (int j = 0; j < SIZE; j++)
				matrix[i][j] = matrix[j][i] = 0;
		}
	}

	//�ж����������Ƿ�����
	//����ĳһ��������ܷ�ֱ�ӵ�����һ����
	for (int i = 0; i < SIZE - 1; i++)
		for (int j = i + 1; j < SIZE ; j++)
			if (visited[i] == 0 && visited[j] == 0 && unavailable(i, j))
				matrix[i][j] = matrix[j][i] = 0;

	//����־λ����Ϊ0
	for (int i = 0; i < SIZE; i++)
		visited[i] = 0;
}

//�ж�ũ���ǡ��ˡ����λ��
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

//�жϲ���ȫ״̬
int unsafe(int state) {
	int judge = 0;
	//�Ǻ��򵥶���һ��
	if (wolf_location(state) == sheep_location(state) && wolf_location(state) != farmer_location(state))
		judge = 1;
	//��Ͳ˵�����һ��
	if (cabbage_location(state) == sheep_location(state) && sheep_location(state) != farmer_location(state))
		judge = 1;
	return judge;
}

//�ǵ���԰�
int case1(int i, int j) {
	if (wolf_location(i) != wolf_location(j)) {
		if (sheep_location(i) == sheep_location(j) && cabbage_location(i) == cabbage_location(j))
			return 1;
		else
			return 0;
	} else
		return 0;
}

//�˵���԰�
int case2(int i, int j) {
	if (cabbage_location(i) != cabbage_location(j)) {
		if (sheep_location(i) == sheep_location(j) && wolf_location(i) == wolf_location(j))
			return 1;
		else
			return 0;
	} else
		return 0;
}

//�򵽴�԰�
int case3(int i, int j) {
	if (sheep_location(i) != sheep_location(j)) {
		if (wolf_location(i) == wolf_location(j) && cabbage_location(i) == cabbage_location(j))
			return 1;
		else
			return 0;
	} else
		return 0;
}

//�ж����������Ƿ�����
int unavailable(int i, int j) {
	int judge = 1;
	// <i,j>�ߴ���
	if (farmer_location(i) != farmer_location(j)) {
		//ũ����Է��ض԰�
		if ((i & 0x07) == (j & 0x07))
			judge = 0;
		//ũ����������ض԰�
		if (case1(i, j) || case2(i, j) || case3(i, j))
			judge = 0;
	}
	return judge;
}

//�����������
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

//���·��
void PrintPath(void) {
	int i;
	int row = 10;
	int col = 30;
	SetCCPos(col * number, row * i);
	printf("����%d��", number + 1);
	for (i = 0; i < top - 1; i++) {
		SetCCPos(col * number, row * i + 2);
		puts(situation[stack[i]]);
		SetCCPos(col * number, row * i + 3);
		puts("--------------");
		SetCCPos(col * number, row * i + 4);
		puts("    С��");
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
	puts("    С��");
	SetCCPos(col * number, row * i + 5);
	puts("--------------");
	SetCCPos(col * number, row * i + 6);
	int opposite = (~(stack[i])) & 0x0F;
	puts(situation[opposite]);
	interface_bottom = ((row * (top - 1) + 7) > interface_bottom) ? (row * (top - 1) + 7) : interface_bottom;
	SetCCPos(0, interface_bottom);
	number++;
}

//�ж�ջ�Ƿ�Ϊ��
int StackIsEmpty(void) {
	return (top == 0) ? 1 : 0;
}

//�ж�ջ�Ƿ�����
int StackIsFull(void) {
	return (top == SIZE) ? 1 : 0;
}

//��ջ
int push(int data) {
	if (StackIsFull())
		return 0;
	else
		stack[top++] = data;
	return 1;
}

//��ջ
int pop(void) {
	if (StackIsEmpty()) {
		printf("False! Stack is Empty.\n");
		exit(0);
	} else
		return stack[--top];
}

//���ù��λ��
void SetCCPos(int x, int y) {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��ע������
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hOut, pos);//ƫ�ƹ��λ��
}