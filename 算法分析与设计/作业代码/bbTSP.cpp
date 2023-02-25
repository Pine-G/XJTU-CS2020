#include <iostream>
#include <queue>
#include <cfloat>
using namespace std;

struct Node {
	float lcost;			//�������õ��½�
	float rcost;			//x[s:n-1]�ж�����С���߷��ú�
	float cost;				//��ǰ����
	int s;					//����㵽��ǰ����·��Ϊx[0:s]
	int *x;					//��ǰ·����x[s+1:n-1]������
	Node(float a, float b, float c, int d, int *e): lcost(a), rcost(b), cost(c), s(d), x(e) {}
	bool operator <(const Node &node)const {
		return node.lcost < lcost;
	}
};

const int n = 5;			//ͼG�Ķ������
int bestp[n];				//���Ž�

//�ڽӾ���
float a[n + 1][n + 1] = {
	0, 0, 0, 0, 0, 0,
	0, -1, 5, 61, 34, 12,
	0, 57, -1, 43, 20, 7,
	0, 39, 42, -1, 8, 21,
	0, 6, 50, 42, -1, 8,
	0, 41, 26, 10, 35, -1
};

//�����ۻ�Ա��������ȶ���ʽ��֧�޽編
float bbTSP(void) {
	//����С���߷���
	float minOut[n + 1];
	float minSum = 0;
	for (int i = 1; i <= n; i++) {
		minOut[i] = FLT_MAX;
		for (int j = 1; j <= n; j++) {
			if (a[i][j] > 0 && a[i][j] < minOut[i])
				minOut[i] = a[i][j];
		}
		if (minOut[i] == FLT_MAX)
			return FLT_MAX;		//�޻�·
		minSum += minOut[i];
	}

	//��ʼ��·��
	int *x = new int[n];
	for (int i = 0; i < n; i++)
		x[i] = i + 1;

	//��ʼ��С����
	priority_queue<Node> MinHeap;
	Node node(0.f, minSum, 0.f, 0, x);

	float bestc = FLT_MAX;		//����ֵ
	bool exist = true;

	//����������
	while (node.lcost < bestc) {
		x = node.x;
		//��ǰ��չ�����Ҷ���ĸ����
		if (node.s == n - 2) {
			if (a[x[n - 2]][x[n - 1]] > 0 && a[x[n - 1]][1] > 0 && node.cost + a[x[n - 2]][x[n - 1]] + a[x[n - 1]][1] < bestc) {
				bestc = node.cost + a[x[n - 2]][x[n - 1]] + a[x[n - 1]][1];
				for (int j = 0; j < n; j++)
					bestp[j] = x[j];
			}
		} else {
			//������ǰ��չ�����ӽ��
			for (int i = node.s + 1; i < n; i++) {
				if (a[x[node.s]][x[i]] > 0) {
					float cost = node.cost + a[x[node.s]][x[i]];
					float rcost = node.rcost - minOut[x[node.s]];
					float lcost = cost + rcost;
					//�������ܺ����Ž⣬������С����
					if (lcost < bestc) {
						int *xx = new int[n];
						for (int j = 0; j < n; j++)
							xx[j] = x[j];
						xx[node.s + 1] = x[i];
						xx[i] = x[node.s + 1];
						MinHeap.push(Node(lcost, rcost, cost, node.s + 1, xx));
					}
				}
			}
		}
		delete[] x;
		//ȡ��һ��չ���
		if (!MinHeap.empty()) {
			node = MinHeap.top();
			MinHeap.pop();
		} else {
			exist = false;
			break;
		}
	}

	//�ͷ��ڴ�
	if (exist)
		delete[] node.x;
	while (!MinHeap.empty()) {
		node = MinHeap.top();
		MinHeap.pop();
		delete[] node.x;
	}

	return bestc;
}

//���Գ���
int main(void) {
	cout << "��С���ã�" << bbTSP() << endl;
	cout << "·����";
	for (int i = 0; i < n; i++)
		cout << bestp[i] << "->";
	cout << bestp[0] << endl;

	return 0;
}