//哈夫曼压缩、解压缩算法（编译码器）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define SIZE 256

//树结点
typedef struct tree_node {
	char code;
	unsigned long weight;
	struct tree_node *parent;
	struct tree_node *left;
	struct tree_node *right;
} node;

//函数原型
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
			case 'a':		//压缩
				compress();
				break;
			case 'b':		//解压
				extract();
				break;
			default:
				puts("Error!!!");
		}
		choice = menu();
	}

	return 0;
}

//主界面
char menu(void) {
	printf("****************************************\n\n");
	printf("     基于哈夫曼编码的文件压缩程序\n\n");
	printf("      (a)压缩         (b)解压\n");
	printf("      (c)退出\n\n");
	printf("****************************************\n\n");
	printf("功能选择：");
	char choice = getchar();
	fflush(stdin);				//清空输入流
	while (!strchr("abcABC", choice)) {
		printf("\n输入有误！请重新输入：");
		choice = getchar();
		fflush(stdin);			//清空输入流
	}
	if (choice < 'a')
		choice -= 'A' - 'a';	//大写字母转小写

	return choice;
}

//压缩文件
bool compress(void) {
	char in_file[50];
	printf("\n待压缩文件名：");
	gets(in_file);
	fflush(stdin);

	FILE *fp = fopen(in_file, "rb");	//以二进制只读模式打开文件
	if (fp == NULL) {
		printf("\n文件不存在！\n\n");
		return false;
	}

	printf("\n压缩中…………\n\n");

	node leaves[SIZE];					//创建叶结点
	for (int i = 0; i < SIZE; i++) {
		leaves[i].weight = 0;			//初始化叶结点
		leaves[i].parent = NULL;
		leaves[i].left = NULL;
		leaves[i].right = NULL;
	}

	unsigned long file_len = 0;			//原文件长度
	unsigned char temp;
	fread(&temp, sizeof(unsigned char), 1, fp);			//读取8bit
	while (!feof(fp)) {
		leaves[temp].weight++;							//统计字符频率
		file_len++;										//统计原文件长度
		fread(&temp, sizeof(unsigned char), 1, fp);		//读取8bit
	}

	fclose(fp);			//关闭文件

	Huffman_Tree(leaves);				//建立哈夫曼树
	char **Code = encoder(leaves);		//生成哈夫曼编码
	free_tree(leaves);					//释放树占用的内存

	char out_file[54];					//生成的压缩文件名
	strcpy(out_file, in_file);
	strcat(out_file, ".huf");

	FILE *fp1 = fopen(in_file, "rb");		//以二进制只读模式打开文件
	FILE *fp2 = fopen(out_file, "wb");		//以二进制只写模式打开文件

	fwrite(&file_len, sizeof(unsigned long), 1, fp2);		//写入文件长度

	//写入字符频率
	for (int i = 0; i < SIZE; i++)
		fwrite(&(leaves[i].weight), sizeof(unsigned long), 1, fp2);

	//写入哈夫曼编码
	char code_buf[SIZE + 8] = "\0";			//哈夫曼编码缓存
	while (!feof(fp1)) {
		fread(&temp, sizeof(unsigned char), 1, fp1);		//读取8bit
		strcat(code_buf, Code[temp]);		//编码字符写入缓存区
		while (strlen(code_buf) >= 8) {
			temp = '\0';
			for (int i = 0; i < 8; i++) {
				temp <<= 1;
				if (code_buf[i] == '1')
					temp |= 1;
			}
			fwrite(&temp, sizeof(unsigned char), 1, fp2);	//写入8bit
			strcpy(code_buf, code_buf + 8);					//去除已处理的前8位
		}
	}
	//处理最后不足8bit编码
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

	fclose(fp1);		//关闭文件
	fclose(fp2);

	for (int i = 0; i < SIZE; i++)			//释放编码字符占用的内存
		free(Code[i]);
	free(Code);

	printf("压缩完成！\n\n");
	return true;
}

//解压文件
bool extract(void) {
	char in_file[54];						//待解压文件名
	printf("\n待解压文件名：");
	gets(in_file);
	fflush(stdin);

	FILE *fp1 = fopen(in_file, "rb");		//以二进制只读模式打开文件
	if (fp1 == NULL) {
		printf("\n文件不存在！\n\n");
		return false;
	}

	printf("\n解压中…………\n\n");

	char out_file[54] = "ext_";				//生成的解压文件名
	strncat(out_file, in_file, strlen(in_file) - 4);

	FILE *fp2 = fopen(out_file, "wb");		//以二进制只写模式打开文件

	unsigned long file_len;					//读取文件长度
	fread(&file_len, sizeof(unsigned long), 1, fp1);

	node leaves[SIZE];
	for (int i = 0; i < SIZE; i++) {		//读取字符频率
		leaves[i].parent = NULL;			//初始化叶结点
		leaves[i].left = NULL;
		leaves[i].right = NULL;
		fread(&(leaves[i].weight), sizeof(unsigned long), 1, fp1);
	}

	Huffman_Tree(leaves);					//建立哈夫曼树
	node *root;								//寻找根结点
	for (int i = 0; i < SIZE; i++) {
		if (leaves[i].weight) {				//频率为0的字符不在哈夫曼树中
			root = &(leaves[i]);			//无法通过其寻找根结点
			break;
		}
	}
	while (root->parent)
		root = root->parent;

	unsigned long written_len = 0;			//写入的编码个数
	unsigned char temp;
	node *leaf = root;
	while (file_len != written_len) {		//二进制文件无法用feof判断文件结束
		fread(&temp, sizeof(unsigned char), 1, fp1);
		for (int i = 0; i < 8; i++) {
			leaf = (temp & 128) ? leaf->right : leaf->left;
			temp <<= 1;
			if (!leaf->left) {				//从根结点遍历至叶结点
				unsigned char ch = leaf - leaves;				//确定编码
				fwrite(&ch, sizeof(unsigned char), 1, fp2);		//写入8bit
				written_len++;
				if (file_len == written_len)
					break;
				leaf = root;
			}
		}
	}

	fclose(fp1);			//关闭文件
	fclose(fp2);
	free_tree(leaves);		//释放树占用的内存

	printf("解压完成！\n\n");
	return true;
}

//构建哈夫曼树
void Huffman_Tree(node *leaves) {
	node *tree[SIZE];					//建立森林
	for (int i = 0; i < SIZE; i++)
		tree[i] = &(leaves[i]);

	//筛选法建立小顶堆
	for (int i = SIZE / 2 - 1; i >= 0; i--) 	//从最后一个内部结点开始
		SiftDown(tree, SIZE, i);

	for (int count = SIZE - 1; count > 0; count--) {
		while (tree[0]->weight == 0) {
			tree[0] = tree[count];
			SiftDown(tree, count, 0);	//调整小顶堆
			count--;
		}
		node *temp1 = tree[0];		//权重最小的元素（非零）

		tree[0] = tree[count];
		SiftDown(tree, count, 0);		//调整小顶堆

		while (tree[0]->weight == 0) {
			tree[0] = tree[count];
			SiftDown(tree, count, 0);	//调整小顶堆
			count--;
		}
		node *temp2 = tree[0];		//权重次小的元素（非零）

		node *root = (node *)malloc(sizeof(node));
		root->weight = temp1->weight + temp2->weight;
		root->parent = NULL;
		root->left = temp1;				//生成子树
		root->right = temp2;
		temp1->parent = temp2->parent = root;
		temp1->code = '0';
		temp2->code = '1';

		tree[0] = root;
		SiftDown(tree, count, 0);		//调整小顶堆
	}
}

//筛选法调整堆
void SiftDown(node **tree, int n, int i) {
	node *temp = tree[i];
	while (2 * i + 1 < n) {			//不断下沉
		int index = 2 * i + 1;
		if (index < n - 1 && tree[index]->weight > tree[index + 1]->weight)
			index++;				//取左右子结点中的较小者
		if (temp->weight <= tree[index]->weight)
			break;					//终止下沉
		else {
			tree[i] = tree[index];
			i = index;
		}
	}
	tree[i] = temp;
}

//哈夫曼编码
char **encoder(node *leaves) {
	char **Code = (char **)malloc(SIZE * sizeof(char *));
	for (int i = 0; i < SIZE; i++) {
		node *temp = &leaves[i];
		//从叶结点向上读取编码
		int j = 0;
		char ch_temp[SIZE];			//临时存储反向编码
		while (temp->parent) {
			ch_temp[j] = temp->code;
			j++;
			temp = temp->parent;
		}
		//获取正向编码
		char *code = (char *)malloc((j + 1) * sizeof(char));
		int m = 0;
		for (int k = j - 1; k >= 0; k--)
			code[m++] = ch_temp[k];
		code[m] = '\0';
		Code[i] = code;
	}
	return Code;
}

//销毁哈夫曼树
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