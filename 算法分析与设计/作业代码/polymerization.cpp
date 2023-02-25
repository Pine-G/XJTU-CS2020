#include <iostream>
#define d 6/*����*/

typedef struct {
	int *coefficient;	//����ʽϵ��
	int n;				//��������
} polynomial;

int zero_point[d] = {1, 2, 3, 4, 5, 6/*��㼯*/};

//������������ʽ�Ŀռ�
void clear(polynomial *p) {
	delete[] p->coefficient;
	delete p;
}

polynomial *Mul1(polynomial *p1, polynomial *p2) {
	polynomial *p = new polynomial;
	p->n = p1->n + 1;
	p->coefficient = new int[p->n + 1];
	p->coefficient[0] = p1->coefficient[0] * p2->coefficient[0];
	for (int i = 1; i <= p->n; i++) {
		p->coefficient[i] = 0;
		for (int j = 0; j < 2; j++) {
			if (i - j <= p1->n && j <= i)
				p->coefficient[i] += p1->coefficient[i - j] * p2->coefficient[j];
		}
	}
	clear(p1);
	clear(p2);
	return p;
}

polynomial *Mul2(polynomial *p1, polynomial *p2) {
	polynomial *p = new polynomial;
	p->n = 2 * p1->n;
	p->coefficient = new int[p->n + 1];
	for (int i = 0; i <= p->n; i++) {
		p->coefficient[i] = 0;
		for (int j = 0; j <= p1->n; j++) {
			if (i - j <= p2->n && j <= i)
				p->coefficient[i] += p1->coefficient[j] * p2->coefficient[i - j];
		}
	}
	clear(p1);
	clear(p2);
	return p;
}

//��ʼ��һ�ζ���ʽP(x)=x-nd
polynomial **initialize(int *zero_point) {
	polynomial **p = new polynomial*[d];
	for (int i = 0; i < d; i++) {
		p[i] = new polynomial;
		p[i]->coefficient = new int[2];
		p[i]->coefficient[0] = -zero_point[i];
		p[i]->coefficient[1] = 1;
		p[i]->n = 1;
	}
	return p;
}

//������ʽ
polynomial *poly(polynomial **p, int low, int high) {
	//Mul1()������O(i)ʱ���ڼ���һ��i�ζ���ʽ��һ��1�ζ���ʽ�ĳ˻�
	//Mul2()������O(ilogi)ʱ���ڼ�������i�ζ���ʽ�ĳ˻�
	polynomial *ans = NULL;
	if (low == high) {
		ans = p[low];
	} else if (high - low == 1) {
		ans = Mul1(p[low], p[high]);
	} else {
		int mid = (low + high) / 2;
		if ((high - low) % 2 == 0) {
			polynomial *p1 = poly(p, low, mid - 1);
			polynomial *p2 = poly(p, mid, high - 1);
			polynomial *p3 = p[high];
			ans = Mul2(p1, p2);
			ans = Mul1(ans, p3);
		} else {
			polynomial *p1 = poly(p, low, mid);
			polynomial *p2 = poly(p, mid + 1, high);
			ans = Mul2(p1, p2);
		}
	}

	return ans;
}

int main(void) {
	polynomial **p = initialize(zero_point);
	polynomial *ans = poly(p, 0, d - 1);
	delete[] p;

	for (int i = 0; i <= ans->n; i++) {
		std::cout << ans->coefficient[i] << ' ';
	}
	clear(ans);

	return 0;
}