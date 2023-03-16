#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

//ҳ����
typedef struct page {
	unsigned int frame;		//֡��
	bool flag;				//���λ
	unsigned int counter;	//������
} page;

page *page_table = NULL;			//ҳ��ָ��
unsigned int *reference = NULL;		//ҳ�����ô�
unsigned int table_size = 10;		//ҳ���С
unsigned int frame_size = 4;		//֡���С
unsigned int ref_size = 20;			//���ô���С
unsigned int belady[12] = { 1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5 };

//��ʼ��ҳ�������λ����Ч������������
void initial_page_table() {
	page_table = (page *)malloc(table_size * sizeof(page));
	if (!page_table) {
		printf("page_table: malloc failed!\n");
		exit(0);
	}
	for (unsigned int i = 0; i < table_size; i++) {
		page_table[i].flag = false;
		page_table[i].counter = 0;
	}
}

//��ʼ�����ô�
void initial_reference() {
	reference = (unsigned int *)malloc(ref_size * sizeof(unsigned int));
	if (!reference) {
		printf("reference: malloc failed!\n");
		exit(0);
	}
	//srand((unsigned int)time(0));		//���������
	for (unsigned int i = 0; i < ref_size; i++)
		reference[i] = rand() % table_size;
}

//������ô�
void print_reference() {
	printf("\nreference:\n");
	for (unsigned int i = 0; i < ref_size; i++) {
		printf("%u ", reference[i]);
	}
	printf("\n");
}

//�ͷ�ҳ������ô����ڴ�
void release() {
	if (page_table)
		free(page_table);
	if (reference && reference != belady)
		free(reference);
}

void Belady(void) {
	ref_size = 12;
	frame_size = 3;
	table_size = 6;
	reference = belady;
}

int FIFO(void) {
	unsigned int diseffect = 0;		//ȱҳ������
	unsigned int front = 0;			//����ͷ
	unsigned int rear = 0;			//����β
	unsigned int queue_size = frame_size + 1;
	unsigned int *queue = (unsigned int *)malloc(queue_size * sizeof(unsigned int));
	if (!queue) {
		printf("queue: malloc failed!\n");
		exit(0);
	}

	for (unsigned int i = 0; i < ref_size; i++) {
		printf("Page %u arrives ", reference[i]);
		//ҳ�治��֡��
		if (!page_table[reference[i]].flag) {
			printf("but not in the frame.\n");
			diseffect++;
			if ((rear + 1) % queue_size == front) {
				//֡������
				page_table[queue[front]].flag = false;
				front = (front + 1) % queue_size;
			}
			//ҳ�滻��
			page_table[reference[i]].flag = true;
			page_table[reference[i]].frame = rear;
			queue[rear] = reference[i];
			rear = (rear + 1) % queue_size;
			//�����ǰ֡������
			printf("frame: ");
			for (unsigned int k = front; k != rear; k = (k + 1) % queue_size)
				printf("|%u| ", queue[k]);
			printf("\n");
		} else {
			//ҳ����֡��
			printf("and in the frame.\n");
		}
	}

	free(queue);
	return diseffect;
}

//�������絽���ҳ��
int find_min(unsigned int *frame) {
	unsigned int min = UINT_MAX, frame_num = 0;
	for (unsigned int i = 0; i < frame_size; i++) {
		if (page_table[frame[i]].counter < min) {
			min = page_table[frame[i]].counter;
			frame_num = i;
		}
	}
	return frame_num;
}

unsigned int LRU(void) {
	unsigned int diseffect = 0;		//ȱҳ������
	unsigned int clock = 0;			//�߼�ʱ��
	unsigned int full = 0;			//֡����ʶ
	unsigned int *frame = (unsigned int *)malloc(frame_size * sizeof(unsigned int));
	if (!frame) {
		printf("frame: malloc failed!\n");
		exit(0);
	}

	for (unsigned int i = 0; i < ref_size; i++) {
		printf("Page %u arrives ", reference[i]);
		//����ʱ�����
		if (clock == UINT_MAX) {
			unsigned int min;
			unsigned int *temp = (unsigned int *)malloc(frame_size * sizeof(unsigned int));
			for (unsigned int i = 0; i < frame_size; i++) {
				min = find_min(frame);
				temp[i] = frame[min];
				page_table[frame[min]].counter = UINT_MAX;
			}
			for (unsigned int i = 0; i < frame_size; i++)
				page_table[temp[i]].counter = i;
			free(temp);
			clock = frame_size;
		}
		page_table[reference[i]].counter = clock;
		clock++;
		//ҳ�治��֡��
		if (!page_table[reference[i]].flag) {
			printf("but not in the frame.\n");
			diseffect++;
			if (full < frame_size) {
				//֡δ��ʱ
				page_table[reference[i]].flag = true;
				page_table[reference[i]].frame = full;
				frame[full] = reference[i];
				full++;
			} else {
				unsigned int replace = find_min(frame);
				//ҳ�滻��
				page_table[frame[replace]].flag = false;
				//ҳ�滻��
				page_table[reference[i]].flag = true;
				page_table[reference[i]].frame = replace;
				frame[replace] = reference[i];
			}
			//�����ǰ֡������
			printf("frame: ");
			for (unsigned int k = 0; k < full; k++)
				printf("|%u| ", frame[k]);
			printf("\n");
		} else {
			//ҳ����֡��
			printf("and in the frame.\n");
		}
	}

	free(frame);
	return diseffect;
}

//���ò���
char set_parameter(void) {
	//�û��㷨��Belady�쳣����
	printf("\nChoice: 1)FIFO      2)LRU      3)FIFO(Belady)\n");
	char choice = getchar();
	while (getchar() != '\n');
	if (choice == '3') {
		Belady();
		return choice;
	}

	unsigned int size = 0;
	//ҳ���С
	printf("Please enter the size(0 < size <= 128) of the page_table: ");
	if (scanf("%u", &size) == 1 && size > 0 && size <= 128)
		table_size = size;
	else {
		table_size = 10;
		printf("default = 10\n");
	}
	while (getchar() != '\n');
	//֡���С
	printf("Please enter the size(0 < size <= 32) of the frame: ");
	if (scanf("%u", &size) == 1 && size > 0 && size <= 32)
		frame_size = size;
	else {
		frame_size = 4;
		printf("default = 4\n");
	}
	while (getchar() != '\n');
	//���ô���С
	printf("Please enter the size(0 <= size <= 100) of the reference: ");
	if (scanf("%u", &size) == 1 && size >= 0 && size <= 100)
		ref_size = size;
	else {
		ref_size = 20;
		printf("default = 20\n");
	}
	while (getchar() != '\n');

	return choice;
}

int main(void) {
	printf("*********************************\n");
	printf("       PAGE REPLACEMENT\n");
	printf("*********************************\n\n");

	char choice1 = set_parameter();
	if (choice1 != '3')
		initial_reference();
	unsigned int diseffect;

	while (true) {
		print_reference();
		initial_page_table();

		switch (choice1) {
			case '1':
				printf("\nFIFO:\n\n");
				diseffect = FIFO();
				break;
			case '2':
				printf("\nLRU:\n\n");
				diseffect = LRU();
				break;
			case '3':
				printf("\nFIFO(Belady):\n\n");
				diseffect = FIFO();
				break;
			default:
				printf("\ndefault = FIFO:\n\n");
				diseffect = FIFO();
		}

		printf("\nPage fault: %d\n", diseffect);
		printf("ratio: %.2f%%\n", (1.0 - (float)diseffect / ref_size) * 100.0);

		printf("\nQuit? [defualt[Enter] = 'No', [y] = 'yes']: ");
		//�˳�����
		if (getchar() == 'y') {
			release();
			break;
		}

		printf("\nReset? [defualt[Enter] = 'No', [y] = 'yes']: ");
		if (getchar() == 'y') {
			while (getchar() != '\n');
			//���ò���
			release();
			choice1 = set_parameter();
			if (choice1 != '3')
				initial_reference();
		} else {
			printf("\n1)Change the algorithm       2)Change the size of frame\n");
			unsigned int size = 0;
			char choice2 = getchar();
			while (getchar() != '\n');
			switch (choice2) {
				case '1':
					//�����㷨
					choice1 = (choice1 == '2') ? '1' : '2';
					break;
				case '2':
					//����֡���С
					printf("\nThe new size(0 < size <= 32) of the frame: ");
					if (scanf("%u", &size) == 1 && size > 0 && size <= 32)
						frame_size = size;
					else {
						frame_size = 4;
						printf("default = 4\n");
					}
					while (getchar() != '\n');
					break;
				default:
					printf("default: Change the algorithm\n");
					choice1 = (choice1 == '2') ? '1' : '2';
			}
		}
	}

	return 0;
}