#include <iostream>
#include <queue>
using namespace std;

const int N = 101;	//Ȩֵ����
const int n = 5;	//�������

//�ڽӾ���-1��ʾ�߲����ڣ�
const int edge[n][n] = {
	{0, 10, -1, 30, 100},
	{-1, 0, 50, -1, -1},
	{-1, -1, 0, -1, 10},
	{-1, -1, 20, 0, 60},
	{-1, -1, -1, -1, 0}
};

void dijkstra(const int &source) {
	//�����������
	if (source <= 0 || source > n) {
		cout << "False input!" << endl;
		return;
	}

	//��ʼ����������
	int dist[n];
	for (int i = 0; i < n; i++)
		dist[i] = -1;
	bool visited[n] = {false};
	queue<int> bucket[N * n];

	bucket[0].push(source - 1);
	int count = 0;
	//����ɨ���������
	for (int i = 0; i < N * n; i++) {
		while (!bucket[i].empty()) {
			//������
			int v = bucket[i].front();
			bucket[i].pop();
			if (visited[v])
				continue;
			//��ȡ���·��
			dist[v] = i;
			visited[v] = true;
			count++;
			if (count == n)
				break;
			//��δ���ʵ����ڶ��������
			for (int j = 0; j < n; j++) {
				if (!visited[j] && edge[v][j] > 0)
					bucket[i + edge[v][j]].push(j);
			}
		}
		if (count == n)
			break;
	}

	//���
	for (int i = 0; i < n; i++)
		cout << "dist[" << i + 1 << "]:" << dist[i] << endl;
}

//���Գ���
int main(void) {
	cout << "source point(from 1-" << n << "):";
	int s;
	cin >> s;
	dijkstra(s);
	return 0;
}