#include <iostream>

int index_select(int *a, int low, int high) {
	int mid = (low + high) / 2;
	if (low == high && low != a[low]) {
		std::cerr << "non-existent!" << std::endl;
		exit(0);
	} else {
		if (mid < a[mid])
			return index_select(a, low, mid - 1);
		else if (mid > a[mid])
			return index_select(a, mid + 1, high);
		else
			return a[mid];
	}
}

//���Գ���
int main(void) {
	//�������źõ�����
	int a[10] = { -1, 0, 1, 2, 3, 4, 5, 7, 17, 18 };
	std::cout << index_select(a, 0, 9) << std::endl;
	return 0;
}