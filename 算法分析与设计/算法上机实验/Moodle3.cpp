#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

// �㷨Ҫ�����һ���㷨�� T ����С���㼯�� S��ʹ T/S Ϊһ�� d ɭ�֡�

//���Ľ��
struct Node {
	float weight;			//�ý�㵽�����ı��ϵ�Ȩֵ
	int parent_id;			//�ý��ĸ������
	int num;				//�ý����ӽ�����
	float max_road = 0;		//�ý�㵽���������·��
	bool cut = false;		//�Ƿ��ڼ��� S ��
};

class dTree {
	public:
		dTree(int a, int b) : n(a), d(b) {
			//�˴�Ϊ���캯������ʼ����������
			node = new Node[n];
			node[0].parent_id = -1;
			node[0].weight = 0;
			for (int i = 0; i < n; i++) {
				cin >> node[i].num;
				for (int j = 0; j < node[i].num; j++) {
					int index;
					cin >> index;
					cin >> node[index].weight;
					node[index].parent_id = i;
				}
			}
		}
		~dTree(void) {
			delete[] node;
		}
		void solution(void) {
			//main����ͨ������dTree���solution������ͨ��cout��������
			int count = 0;
			for (int i = n - 1; i >= 0; i--) {
				if (node[i].max_road > d) {
					//ɾ�����
					node[i].cut = true;
					count++;
				} else {
					//���¸����
					int par = node[i].parent_id;
					if (par >= 0 && node[par].max_road < node[i].max_road + node[i].weight)
						node[par].max_road = node[i].max_road + node[i].weight;
				}
			}
			cout << count;
		}
	private:
		int n;			//������
		int d;			//���·������
		Node *node;		//�������
};

//��Ĵ���ֻ��Ҫ��ȫ�Ϸ�dTree����ʵ���㷨

//������Ҫ�������������������Լ������д

//ֻ��Ҫ�ύ�⼸�д��룬�����Ķ��Ǻ�̨ϵͳ�Զ���ɵģ������� LeetCode������Ϊmain�����Ĵ���


int main() {
	int n, d;			//nΪ���������dΪ·������
	cin >> n >> d;
	dTree dt(n, d);    	//�������ʼ����
	dt.solution();      //ͨ��solution����������
	return 0;
}