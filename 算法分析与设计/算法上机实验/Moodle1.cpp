#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

// �㷨Ҫ�����дһ�δ��룬�ܹ����������� O(n)ʱ���ҳ� n ��Ԫ�صĴ�Ȩ��λ��

// ����ԭ�ͣ�

void WeightMedian(int length, vector<int> num, vector<double> weight, int index) {
	if (length == 1) {
		cout << num[index];
		return;
	}

	//��Ԫ��������ֳ�����
	int low = index, high = length + index - 1;
	int temp = rand() % (length - 1) + index + 1;	//�������ѡ��Ľ������Ԫ��
	swap(num[low], num[temp]);
	swap(weight[low], weight[temp]);
	int pivot = num[low];
	double pivot_w = weight[low];
	while (low < high) {
		while (low < high && num[high] > pivot)
			--high;
		num[low] = num[high];
		weight[low] = weight[high];
		while (low < high && num[low] <= pivot)
			++low;
		num[high] = num[low];
		weight[high] = weight[low];
	}
	num[low] = pivot;
	weight[low] = pivot_w;

	//�ֱ�������������Ȩ�غ�
	double left_weight = 0, right_weight = 0;
	for (int i = index; i < low; i++)
		left_weight += weight[i];
	for (int i = low + 1; i < length + index; i++)
		right_weight += weight[i];

	//�ݹ�
	if (left_weight <= 0.5 && right_weight <= 0.5)
		cout << num[low];
	else if (left_weight > right_weight) {
		weight[low] += right_weight;
		WeightMedian(low - index + 1, num, weight, index);
	} else {
		weight[low] += left_weight;
		WeightMedian(length - low + index, num, weight, low);
	}
}

//��Ĵ���ֻ��Ҫ��ȫ�Ϸ�������ʵ���㷨

//����lengthΪ���볤�ȣ�num�ǰ���n��������ͬԪ��ֵ��������weight�ǰ���Ԫ��ֵ��Ӧ��Ȩ�ص�������indexΪ�ݹ����ʱ������(�±�)

//ֻ��Ҫ�ύ�⼸�д��룬�����Ķ��Ǻ�̨ϵͳ�Զ���ɵġ������� LeetCode

int main() {

	// ��̨�Զ��������Դ�������������ͬѧ��д

	//���Դ��뽫�����������������ݷֱ���length��num����weight��

	//����WeightMedian(length,num, weight,index)�����������ڲ�ʹ��cout����õ�����λ�������Դ���Ĭ��index��ʼֵΪ0

	vector<int> num = { 719, 449, 446, 981, 431, 993, 919, 389, 549, 453 };
	vector<double> weight = { 0.01757775, 0.02028202, 0.16863048, 0.07320842, 0.16283562, 0.16167665, 0.14970060, 0.04095036, 0.12806645, 0.07707166 };
	int n = num.size();
	WeightMedian(n, num, weight, 0);

	return 0;

}