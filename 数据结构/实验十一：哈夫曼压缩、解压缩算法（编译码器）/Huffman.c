//������ѹ������ѹ���㷨������������
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define SIZE 256

//�����
typedef struct tree_node {
	char code;
	unsigned long weight;
	struct tree_node *parent;
	struct tree_node *left;
	struct tree_node *right;
} node;

//����ԭ��
char menu(void);
bool compress(void);
bool extract(void);
void Huffman_Tree(node *leaves);
char **encoder(node *leaves);
void free_tree(node *leaves);
void SiftDown(node **tree, int n, int i);

int main(void) {
	char choice = menu();
	while (choice != 'c') {
		switch (choice) {
			case 'a':		//ѹ��
				compress();
				break;
			case 'b':		//��ѹ
				extract();
				break;
			default:
				puts("Error!!!");
		}
		choice = menu();
	}

	return 0;
}

//������
char menu(void) {
	printf("****************************************\n\n");
	printf("     ���ڹ�����������ļ�ѹ������\n\n");
	printf("      (a)ѹ��         (b)��ѹ\n");
	printf("      (c)�˳�\n\n");
	printf("****************************************\n\n");
	printf("����ѡ��");
	char choice = getchar();
	fflush(stdin);				//���������
	while (!strchr("abcABC", choice)) {
		printf("\n�����������������룺");
		choice = getchar();
		fflush(stdin);			//���������
	}
	if (choice < 'a')
		choice -= 'A' - 'a';	//��д��ĸתСд

	return choice;
}

//ѹ���ļ�
bool compress(void) {
	char in_file[50];
	printf("\n��ѹ���ļ�����");
	gets(in_file);
	fflush(stdin);

	FILE *fp = fopen(in_file, "rb");	//�Զ�����ֻ��ģʽ���ļ�
	if (fp == NULL) {
		printf("\n�ļ������ڣ�\n\n");
		return false;
	}

	printf("\nѹ���С�������\n\n");

	node leaves[SIZE];					//����Ҷ���
	for (int i = 0; i < SIZE; i++) {
		leaves[i].weight = 0;			//��ʼ��Ҷ���
		leaves[i].parent = NULL;
		leaves[i].left = NULL;
		leaves[i].right = NULL;
	}

	unsigned long file_len = 0;			//ԭ�ļ�����
	unsigned char temp;
	fread(&temp, sizeof(unsigned char), 1, fp);			//��ȡ8bit
	while (!feof(fp)) {
		leaves[temp].weight++;							//ͳ���ַ�Ƶ��
		file_len++;										//ͳ��ԭ�ļ�����
		fread(&temp, sizeof(unsigned char), 1, fp);		//��ȡ8bit
	}

	fclose(fp);			//�ر��ļ�

	Huffman_Tree(leaves);				//������������
	char **Code = encoder(leaves);		//���ɹ���������
	free_tree(leaves);					//�ͷ���ռ�õ��ڴ�

	char out_file[54];					//���ɵ�ѹ���ļ���
	strcpy(out_file, in_file);
	strcat(out_file, ".huf");

	FILE *fp1 = fopen(in_file, "rb");		//�Զ�����ֻ��ģʽ���ļ�
	FILE *fp2 = fopen(out_file, "wb");		//�Զ�����ֻдģʽ���ļ�

	fwrite(&file_len, sizeof(unsigned long), 1, fp2);		//д���ļ�����

	//д���ַ�Ƶ��
	for (int i = 0; i < SIZE; i++)
		fwrite(&(leaves[i].weight), sizeof(unsigned long), 1, fp2);

	//д�����������
	char code_buf[SIZE + 8] = "\0";			//���������뻺��
	while (!feof(fp1)) {
		fread(&temp, sizeof(unsigned char), 1, fp1);		//��ȡ8bit
		strcat(code_buf, Code[temp]);		//�����ַ�д�뻺����
		while (strlen(code_buf) >= 8) {
			temp = '\0';
			for (int i = 0; i < 8; i++) {
				temp <<= 1;
				if (code_buf[i] == '1')
					temp |= 1;
			}
			fwrite(&temp, sizeof(unsigned char), 1, fp2);	//д��8bit
			strcpy(code_buf, code_buf + 8);					//ȥ���Ѵ����ǰ8λ
		}
	}
	//���������8bit����
	int length = strlen(code_buf);
	if (length > 0) {
		temp = '\0';
		for (int i = 0; i < length; i++) {
			temp <<= 1;
			if (code_buf[i] == '1')
				temp |= 1;
		}
		temp <<= 8 - length;
		fwrite(&temp, sizeof(unsigned char), 1, fp2);
	}

	fclose(fp1);		//�ر��ļ�
	fclose(fp2);

	for (int i = 0; i < SIZE; i++)			//�ͷű����ַ�ռ�õ��ڴ�
		free(Code[i]);
	free(Code);

	printf("ѹ����ɣ�\n\n");
	return true;
}

//��ѹ�ļ�
bool extract(void) {
	char in_file[54];						//����ѹ�ļ���
	printf("\n����ѹ�ļ�����");
	gets(in_file);
	fflush(stdin);

	FILE *fp1 = fopen(in_file, "rb");		//�Զ�����ֻ��ģʽ���ļ�
	if (fp1 == NULL) {
		printf("\n�ļ������ڣ�\n\n");
		return false;
	}

	printf("\n��ѹ�С�������\n\n");

	char out_file[54] = "ext_";				//���ɵĽ�ѹ�ļ���
	strncat(out_file, in_file, strlen(in_file) - 4);

	FILE *fp2 = fopen(out_file, "wb");		//�Զ�����ֻдģʽ���ļ�

	unsigned long file_len;					//��ȡ�ļ�����
	fread(&file_len, sizeof(unsigned long), 1, fp1);

	node leaves[SIZE];
	for (int i = 0; i < SIZE; i++) {		//��ȡ�ַ�Ƶ��
		leaves[i].parent = NULL;			//��ʼ��Ҷ���
		leaves[i].left = NULL;
		leaves[i].right = NULL;
		fread(&(leaves[i].weight), sizeof(unsigned long), 1, fp1);
	}

	Huffman_Tree(leaves);					//������������
	node *root;								//Ѱ�Ҹ����
	for (int i = 0; i < SIZE; i++) {
		if (leaves[i].weight) {				//Ƶ��Ϊ0���ַ����ڹ���������
			root = &(leaves[i]);			//�޷�ͨ����Ѱ�Ҹ����
			break;
		}
	}
	while (root->parent)
		root = root->parent;

	unsigned long written_len = 0;			//д��ı������
	unsigned char temp;
	node *leaf = root;
	while (file_len != written_len) {		//�������ļ��޷���feof�ж��ļ�����
		fread(&temp, sizeof(unsigned char), 1, fp1);
		for (int i = 0; i < 8; i++) {
			leaf = (temp & 128) ? leaf->right : leaf->left;
			temp <<= 1;
			if (!leaf->left) {				//�Ӹ���������Ҷ���
				unsigned char ch = leaf - leaves;				//ȷ������
				fwrite(&ch, sizeof(unsigned char), 1, fp2);		//д��8bit
				written_len++;
				if (file_len == written_len)
					break;
				leaf = root;
			}
		}
	}

	fclose(fp1);			//�ر��ļ�
	fclose(fp2);
	free_tree(leaves);		//�ͷ���ռ�õ��ڴ�

	printf("��ѹ��ɣ�\n\n");
	return true;
}

//������������
void Huffman_Tree(node *leaves) {
	node *tree[SIZE];					//����ɭ��
	for (int i = 0; i < SIZE; i++)
		tree[i] = &(leaves[i]);

	//ɸѡ������С����
	for (int i = SIZE / 2 - 1; i >= 0; i--) 	//�����һ���ڲ���㿪ʼ
		SiftDown(tree, SIZE, i);

	for (int count = SIZE - 1; count > 0; count--) {
		while (tree[0]->weight == 0) {
			tree[0] = tree[count];
			SiftDown(tree, count, 0);	//����С����
			count--;
		}
		node *temp1 = tree[0];		//Ȩ����С��Ԫ�أ����㣩

		tree[0] = tree[count];
		SiftDown(tree, count, 0);		//����С����

		while (tree[0]->weight == 0) {
			tree[0] = tree[count];
			SiftDown(tree, count, 0);	//����С����
			count--;
		}
		node *temp2 = tree[0];		//Ȩ�ش�С��Ԫ�أ����㣩

		node *root = (node *)malloc(sizeof(node));
		root->weight = temp1->weight + temp2->weight;
		root->parent = NULL;
		root->left = temp1;				//��������
		root->right = temp2;
		temp1->parent = temp2->parent = root;
		temp1->code = '0';
		temp2->code = '1';

		tree[0] = root;
		SiftDown(tree, count, 0);		//����С����
	}
}

//ɸѡ��������
void SiftDown(node **tree, int n, int i) {
	node *temp = tree[i];
	while (2 * i + 1 < n) {			//�����³�
		int index = 2 * i + 1;
		if (index < n - 1 && tree[index]->weight > tree[index + 1]->weight)
			index++;				//ȡ�����ӽ���еĽ�С��
		if (temp->weight <= tree[index]->weight)
			break;					//��ֹ�³�
		else {
			tree[i] = tree[index];
			i = index;
		}
	}
	tree[i] = temp;
}

//����������
char **encoder(node *leaves) {
	char **Code = (char **)malloc(SIZE * sizeof(char *));
	for (int i = 0; i < SIZE; i++) {
		node *temp = &leaves[i];
		//��Ҷ������϶�ȡ����
		int j = 0;
		char ch_temp[SIZE];			//��ʱ�洢�������
		while (temp->parent) {
			ch_temp[j] = temp->code;
			j++;
			temp = temp->parent;
		}
		//��ȡ�������
		char *code = (char *)malloc((j + 1) * sizeof(char));
		int m = 0;
		for (int k = j - 1; k >= 0; k--)
			code[m++] = ch_temp[k];
		code[m] = '\0';
		Code[i] = code;
	}
	return Code;
}

//���ٹ�������
void free_tree(node *leaves) {
	for (int i = 0; i < SIZE; i++) {
		node *temp1 = leaves[i].parent;
		node *temp2;
		while (temp1) {
			temp2 = temp1->parent;
			free(temp1);
			temp1 = temp2;
		}
	}
}