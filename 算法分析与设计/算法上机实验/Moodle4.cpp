#include <iostream>
#include <cmath>
#define ROW 1000
#define COL 20
#define A -1
#define B 1
using namespace std;

int matrix[ROW][COL];
bool mutex[COL][COL];
int result[COL];
int best[COL];
int now_sum;
int max_sum;

//�жϵ�i�к͵�j���Ƿ񻥳�
void judge(void) {
	for (int i = 0; i < COL; i++)
		for (int j = 0; j < COL; j++)
			mutex[i][j] = true;

	for (int i = 0; i < COL - 1; i++) {
		for (int j = i + 1; j < COL; j++) {
			bool tab = false;
			for (int k = 0; k < ROW; k++) {
				if (matrix[k][i] && matrix[k][j]) {
					tab = true;
					break;
				}
			}
			if (tab)
				mutex[i][j] = mutex[j][i] = false;
		}
	}
}

//�жϼ����Ƿ�Ϊ��
bool empty(int k) {
	for (int i = 0; i < COL; i++)
		if (result[i] == k)
			return false;
	return true;
}

//Լ������
//��j�п��Է��뼯��A��k=-1����B��k=1����
bool constraint(int k, int j) {
	for (int i = 0; i < COL; i++) {
		if (result[i] != -k)
			continue;
		else if (mutex[i][j])
			continue;
		else
			return false;
	}
	return true;
}

//���㼯��Ԫ�ظ���
int count(int k, int *array) {
	int sum = 0;
	for (int i = 0; i < COL; i++) {
		if (array[i] == k)
			sum++;
	}
	return sum;
}

//�������
//���������һ��ΪT1���ڶ���ΪT2
//T1��Ԫ�ظ�������T2
//��Ԫ�ظ�����ͬ����T1��Ԫ�غ�С��T2
int output_control(void) {
	int A_num = count(A, best), B_num = count(B, best);
	if (A_num > B_num)
		return A;
	else if (A_num < B_num)
		return B;
	else {
		int A_sum = 0, B_sum = 0;
		for (int i = 0; i < COL; i++) {
			if (best[i] == A)
				A_sum += i;
			else if (best[i] == B)
				B_sum += i;
		}
		return (A_sum < B_sum) ? A : B;
	}
}

//���ݷ�
void backtrack(int t) {
	if (t == COL) {
		if (!empty(A) && !empty(B)) {
			if (now_sum > max_sum) {
				max_sum = now_sum;
				for (int i = 0; i < COL; i++)
					best[i] = result[i];
			}
			//A��BԪ�ظ�����ľ���ֵ��С
			else if (now_sum == max_sum) {
				int a1 = count(A, best), b1 = count(B, best);
				int a2 = count(A, result), b2 = count(B, result);
				if (abs(a1 - b1) > abs(a2 - b2))
					for (int i = 0; i < COL; i++)
						best[i] = result[i];
			}
		}
	} else {
		if (constraint(A, t)) {
			result[t] = A;
			now_sum++;
			backtrack(t + 1);
			now_sum--;
			result[t] = 0;
		}
		if (constraint(B, t)) {
			result[t] = B;
			now_sum++;
			backtrack(t + 1);
			now_sum--;
			result[t] = 0;
		}
		backtrack(t + 1);
	}
}

int main(void) {
	//����
	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
			cin >> matrix[i][j];

	//���
	judge();
	backtrack(0);

	//���
	int k = output_control();
	for (int i = 0; i < COL; i++)
		if (best[i] == k)
			cout << i << ' ';
	cout << endl;
	for (int i = 0; i < COL; i++)
		if (best[i] == -k)
			cout << i << ' ';

	return 0;
}