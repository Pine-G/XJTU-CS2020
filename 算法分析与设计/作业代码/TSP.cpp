#include <iostream>
#include <cfloat>
using namespace std;

const int n = 5;			//ͼG�Ķ������
int x[n + 1];				//��ǰ��
int best_x[n + 1];			//��ǰ���Ž�
float cost = 0;				//��ǰ����
float min_cost = FLT_MAX;	//��ǰ����ֵ
float min_x[n + 1];			//�Ӷ���i�����ıߵ���С����

int count = 0;				//��¼�ݹ����

//�ڽӾ���
float a[n + 1][n + 1] = {
	0, 0, 0, 0, 0, 0,
	0, -1, 5, 61, 34, 12,
	0, 57, -1, 43, 20, 7,
	0, 39, 42, -1, 8, 21,
	0, 6, 50, 42, -1, 8,
	0, 41, 26, 10, 35, -1
};

//��Ӹ����㷢���ıߵ���С����
void x_min(void) {
	for (int i = 1; i <= n; i++) {
		min_x[i] = FLT_MAX;
		for (int j = 1; j <= n; j++) {
			if (j != i && a[i][j] < min_x[i])
				min_x[i] = a[i][j];
		}
	}
}

//Լ������
bool constraint(int t) {
	return a[x[t - 1]][x[t]] > 0;
}

//�޽纯��
bool bound(int t) {
	float sum_min = 0;
	for (int i = t; i <= n; i++)
		sum_min += min_x[x[i]];
	return min_cost > (cost + a[x[t - 1]][x[t]] + sum_min);
}

/*
//�̲��ϵ��޽纯��
bool bound(int t) {
	return min_cost > (cost + a[x[t - 1]][x[t]]);
}
*/

void backtrack(int t) {
	::count++;
	if (t == n) {
		if (a[x[n - 1]][x[n]] > 0 && a[x[n]][1] > 0 && min_cost > cost + a[x[n - 1]][x[n]] + a[x[n]][1]) {
			for (int i = 1; i <= n; i++)
				best_x[i] = x[i];
			min_cost = cost + a[x[n - 1]][x[n]] + a[x[n]][1];
		}
	} else {
		for (int i = t; i <= n; i++) {
			//�Ƿ���Խ���x[t]����
			if (constraint(t) && bound(t)) {
				swap(x[t], x[i]);
				cost += a[x[t - 1]][x[t]];
				backtrack(t + 1);
				cost -= a[x[t - 1]][x[t]];
				swap(x[t], x[i]);
			}
		}
	}
}

float tsp(void) {
	for (int i = 1; i <= n; i++)
		x[i] = i;
	x_min();
	backtrack(2);
	return min_cost;
}

//���Գ���
int main(void) {
	cout << "��С���ã�" << tsp() << endl;
	cout << "·����";
	for (int i = 1; i <= n; i++)
		cout << best_x[i] << "->";
	cout << best_x[1] << endl;
	cout << "�ݹ������" << ::count << endl;

	return 0;
}