#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

// �㷨Ҫ�����дһ���㷨���ܹ�ȷ��һ���и����ʹ�и���ܴ�����С��

// ����ԭ�ͣ�

void MinCost(int L, int n, int *p) {
	//���и��λ�ð���������
	sort(p, p + n + 2);

	//�����ά����ռ䣬c[i][j]��ʾ��λ��i��λ��j����С�и����
	int **c = new int *[n + 2];
	for (int i = 0; i < n + 2; i++)
		c[i] = new int[n + 2];

	//Ϊ��С�θ������и���۸���ֵ0
	for (int i = 0; i < n + 1; i++) {
		c[i][i] = 0;
		c[i][i + 1] = 0;
	}
	c[n + 1][n + 1] = 0;

	//�Ե��������������
	for (int r = 3; r <= n + 2; r++) {
		for (int i = 0; i <= n - r + 2; i++) {
			int j = i + r - 1;
			c[i][j] = c[i + 1][j] + p[j] - p[i];
			for (int k = i + 2; k < j; k++) {
				int t = c[i][k] + c[k][j] + p[j] - p[i];
				if (t < c[i][j])
					c[i][j] = t;
			}
		}
	}

	//������
	cout << c[0][n + 1];

	//�ͷ��ڴ�ռ�
	for (int i = 0; i < n + 2; i++)
		delete[] c[i];
	delete[] c;
}

//��Ĵ���ֻ��Ҫ��ȫ�Ϸ�������ʵ���㷨,�ɸ����Լ���Ҫ������ĺ���

//����L�Ǹ������ȣ�n��λ�õ������p����n ���и���λ�ã�����

//ֻ��Ҫ�ύ�⼸�д��룬�����Ķ��Ǻ�̨ϵͳ�Զ���ɵġ������� LeetCode

int main() {

	// ��̨�Զ��������Դ�������������ͬѧ��д

	int L, n;
	cin >> L >> n;
	int *p;
	p = new int[n + 2];
	p[0] = 0;
	p[n + 1] = L;
	for (int i = 1; i < n + 1; i++) {
		cin >> p[i];
	}

	MinCost(L, n, p); //���ú������һ���и���С�Ĵ��ۺͣ����ͨ��cout�������Ϊint����

	return 0;

}