#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

//������ҵ����ʱ��
vector<float> a = {2, 14, 4, 16, 6, 5, 3};

struct work {
	int id;			//��ҵ���
	float time;		//���账��ʱ��
	work(int a, float b): id(a), time(b) {}
	bool operator <(const work &x)const {
		return x.time < time;
	}
};

struct machine {
	int id;			//�������
	float avail;	//��ɵ�ǰ��ҵ����ʱ��
	machine(int a, float b): id(a), avail(b) {}
	bool operator <(const machine &x)const {
		return x.avail < avail;
	}
};

//̰���㷨
float LPT(int m) {
	int n = a.size();
	if (n <= m) {
		cout << "Ϊÿ����ҵ����һ̨����" << endl;
		float total_time = a[0];
		for (int i = 1; i < n; i++)
			total_time = (a[i] > total_time) ? a[i] : total_time;
		return total_time;
	}

	//��ʼ����ҵ
	vector<work> job;
	for (int i = 0; i < n; i++)
		job.push_back(work(i + 1, a[i]));
	//����ҵ����ʱ�併������
	sort(job.begin(), job.end());

	//��ʼ������������С����
	priority_queue<machine> MinHeap;
	for (int i = 0; i < m; i++)
		MinHeap.push(machine(i + 1, 0));

	//ȡ�Ѷ����������乤��
	for (int i = 0; i < n; i++) {
		machine x = MinHeap.top();
		MinHeap.pop();
		cout << "������" << x.id << "��" << x.avail << "��" << x.avail + job[i].time
		     << "��ʱ��η��������" << job[i].id << endl;
		x.avail += job[i].time;
		MinHeap.push(x);
	}

	//ȡ�����ʱ��
	for (int i = 0; i < m - 1; i++)
		MinHeap.pop();
	return MinHeap.top().avail;
}

int main(void) {
	int t = LPT(3);
	cout << "���ʱ�䣺" << t << endl;

	return 0;
}