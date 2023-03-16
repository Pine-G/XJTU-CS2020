//��EXT2�ļ�ϵͳ���߼�����������С��Ϊ512B������ֻ��һ���û���ֻ����һ���飬ʡ�Գ�����

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define VOLUME_NAME "EXT2FS"				//����
#define EXT2_NAME_LEN 255					//�ļ�����󳤶�
#define FOPEN_TABLE_MAX 16					//�ļ��򿪱�����С
#define BLOCK_SIZE 512						//���ݿ��С
#define DATA_BLOCK_COUNT 4096				//���ݿ����
#define INODE_COUNT 4096					//inode����
#define DISK_SIZE 4611						//���̿����
#define READ_DISK 1							//������
#define WRITE_DISK 0						//д����
#define GDT_START 0							//����������ʼƫ��
#define BLOCK_BITMAP_START 512				//���ݿ�λͼ��ʼƫ��
#define INODE_BITMAP_START 1024				//inodeλͼ��ʼƫ��
#define INODE_TABLE_START 1536				//inode����ʼƫ��
#define DATA_BLOCK_START (1536+512*512)		//���ݿ���ʼƫ��

//�ļ�����
enum {
	FT_UNKNOWN,								//δ֪
	FT_REG_FILE,							//��ͨ�ļ�
	FT_DIR,									//Ŀ¼
	FT_CHRDEV,								//�ַ��豸
	FT_BLKDEV,								//���豸
	FT_FIFO,								//�ܵ�
	FT_SOCK,								//�׽���
	FT_SYMLINK,								//����ָ��
};

//��������(32B)
typedef struct group_desc {
	char bg_volume_name[10];				//����
	unsigned short bg_block_bitmap;			//�������ݿ�λͼ�Ŀ��
	unsigned short bg_inode_bitmap;			//����inodeλͼ�Ŀ��
	unsigned short bg_inode_table;			//inode�����ʼ���
	unsigned short bg_free_blocks_count;	//������п����
	unsigned short bg_free_inodes_count;	//�������inode����
	unsigned short bg_used_dirs_count;		//����Ŀ¼����
	char bg_password[10];					//��¼����
} group_desc;

//�������(64B)
typedef struct inode {
	unsigned short i_mode;					//�ļ����ͼ�����Ȩ��
	unsigned short i_blocks;				//�ļ������ݿ����
	unsigned int i_size;					//�ļ���Ŀ¼�Ĵ�С���ֽڣ�
	unsigned int i_atime;					//����ʱ��
	unsigned int i_ctime;					//����ʱ��
	unsigned int i_mtime;					//�޸�ʱ��
	unsigned int i_dtime;					//ɾ��ʱ��
	unsigned short i_block[8];				//ָ�����ݿ��ָ��
	char i_pad[24];							//��䣨0xff��
} inode;

//Ŀ¼��䳤7-261B��
typedef struct dir_entry {
	unsigned short inode;					//��������
	unsigned short rec_len;					//Ŀ¼���
	unsigned char name_len;					//�ļ�������
	unsigned char file_type;				//�ļ�����
	char name[EXT2_NAME_LEN];				//�ļ���
} dir_entry;

//��д������
group_desc gdt;								//��������������
inode inode_buf;							//������㻺����
dir_entry dir_buf;							//Ŀ¼�����
unsigned char block_bitmap[BLOCK_SIZE];		//��λͼ������
unsigned char inode_bitmap[BLOCK_SIZE];		//inodeλͼ������
unsigned char gdt_Buffer[BLOCK_SIZE];		//����������д������
unsigned char inode_Buffer[BLOCK_SIZE];		//����������ݿ黺����
unsigned char Buffer[BLOCK_SIZE * 2];		//���ݿ黺����

/* Buffer�ĺ�벿�ֲ����洢ʵ�����ݣ�ֻ��������ֹд������ʱ��� */

FILE *fp = NULL;							//�������
char current_path[256];						//��ǰ·�����ַ�����
unsigned short current_dir;					//��ǰĿ¼��������㣩
unsigned short fopen_table[FOPEN_TABLE_MAX];//�ļ��򿪱�
char search_file_name[EXT2_NAME_LEN];		//�����ҵ��ļ���


/************************IO����************************/

//����I/O���Կ�Ϊ��λ��
void disk_IO(long offset, void *buf, int rw_flag) {
	fseek(fp, offset, SEEK_SET);
	if (rw_flag) {
		fread(buf, BLOCK_SIZE, 1, fp);
	} else {
		fwrite(buf, BLOCK_SIZE, 1, fp);
		fflush(fp);
	}
}

//������������
void load_group_desc(void) {
	disk_IO(GDT_START, gdt_Buffer, READ_DISK);
	gdt = ((group_desc *)gdt_Buffer)[0];
}

//������������
void update_group_desc(void) {
	memset(gdt_Buffer, 0xff, sizeof(gdt_Buffer));
	((group_desc *)gdt_Buffer)[0] = gdt;
	disk_IO(GDT_START, gdt_Buffer, WRITE_DISK);
}

/* inode��1��ʼ������0��ʾNULL�����ݿ��0��ʼ���� */

//�����k��inode
void load_inode_entry(unsigned short k) {
	--k;
	unsigned short i = k / 8, j = k % 8;
	disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, READ_DISK);
	inode_buf = ((inode *)inode_Buffer)[j];
}

//���µ�k��inode
void update_inode_entry(unsigned short k) {
	--k;
	unsigned short i = k / 8, j = k % 8;
	disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, READ_DISK);
	((inode *)inode_Buffer)[j] = inode_buf;
	disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, WRITE_DISK);
}

//�����i�����ݿ�
void load_block_entry(unsigned short i) {
	disk_IO(DATA_BLOCK_START + i * BLOCK_SIZE, Buffer, READ_DISK);
}

//���µ�i�����ݿ�
void update_block_entry(unsigned short i) {
	disk_IO(DATA_BLOCK_START + i * BLOCK_SIZE, Buffer, WRITE_DISK);
}

//�������ݿ�λͼ
void load_block_bitmap(void) {
	disk_IO(BLOCK_BITMAP_START, block_bitmap, READ_DISK);
}

//�������ݿ�λͼ
void update_block_bitmap(void) {
	disk_IO(BLOCK_BITMAP_START, block_bitmap, WRITE_DISK);
}

//����inodeλͼ
void load_inode_bitmap(void) {
	disk_IO(INODE_BITMAP_START, inode_bitmap, READ_DISK);
}

//����inodeλͼ
void update_inode_bitmap(void) {
	disk_IO(INODE_BITMAP_START, inode_bitmap, WRITE_DISK);
}


/*************************�ײ�*************************/

//����λͼ�еĵ�һ��0��λ��
int bitmap_find_0(unsigned char bitmap[]) {
	for (int i = 0; i < BLOCK_SIZE; ++i) {
		unsigned char mask = 0b10000000;
		if (bitmap[i] == 0b11111111)
			continue;
		for (int j = 0; j < 8; ++j) {
			if (!(bitmap[i] & mask))
				return i * 8 + j;
			mask >>= 1;
		}
	}
	return -1;
}

//��λͼ�еĵ�kλȡ��
void bitmap_neg_k(unsigned char bitmap[], int k) {
	int i = k / 8, j = k % 8;
	unsigned char mask = 0b10000000;
	for (int t = 0; t < j; ++t)
		mask >>= 1;
	bitmap[i] ^= mask;
}

//��ʼ��inode
void initialize_inode(void) {
	inode_buf.i_mode = FT_UNKNOWN;
	inode_buf.i_mode <<= 8;
	//Ĭ�ϵķ���Ȩ�ޣ��ɶ���д����ִ��
	((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000110;
	inode_buf.i_blocks = 0;
	inode_buf.i_size = 0;
	inode_buf.i_atime = time(NULL);
	inode_buf.i_ctime = time(NULL);
	inode_buf.i_mtime = time(NULL);
	inode_buf.i_dtime = 0;
	memset(inode_buf.i_block, 0, sizeof(inode_buf.i_block));
	memset(inode_buf.i_pad, 0xff, sizeof(inode_buf.i_pad));
}

//����inode
unsigned short new_inode(void) {
	load_group_desc();
	if (gdt.bg_free_inodes_count) {
		//����inodeλͼ
		load_inode_bitmap();
		unsigned short i = bitmap_find_0(inode_bitmap);
		bitmap_neg_k(inode_bitmap, i);
		update_inode_bitmap();
		//����inode��
		initialize_inode();
		update_inode_entry(i + 1);
		//������������
		gdt.bg_free_inodes_count--;
		update_group_desc();
		return i + 1;
	} else {
		printf("There is no inode to be allocated!\n");
		return 0;
	}
}

//�ͷ�inode
void free_inode(unsigned short i) {
	//����inodeλͼ
	load_inode_bitmap();
	bitmap_neg_k(inode_bitmap, i - 1);
	update_inode_bitmap();
	//������������
	load_group_desc();
	gdt.bg_free_inodes_count++;
	update_group_desc();
}

//�������ݿ�
unsigned short new_block(void) {
	load_group_desc();
	if (gdt.bg_free_blocks_count) {
		//�������ݿ�λͼ
		load_block_bitmap();
		unsigned short i = bitmap_find_0(block_bitmap);
		bitmap_neg_k(block_bitmap, i);
		update_block_bitmap();
		//������������
		gdt.bg_free_blocks_count--;
		update_group_desc();
		return i;
	} else {
		printf("There is no block to be allocated!\n");
		return 0;
	}
}

//�ͷ����ݿ�
void free_block(unsigned short i) {
	//�������ݿ�λͼ
	load_block_bitmap();
	bitmap_neg_k(block_bitmap, i);
	update_block_bitmap();
	//������������
	load_group_desc();
	gdt.bg_free_blocks_count++;
	update_group_desc();
}

//����Ŀ¼��
void new_dir_entry(char file_name[], unsigned char file_type) {
	memset(&dir_buf, 0, sizeof(dir_entry));
	dir_buf.inode = new_inode();
	dir_buf.name_len = strlen(file_name);
	dir_buf.rec_len = 7 + dir_buf.name_len;
	dir_buf.file_type = file_type;
	strcpy(dir_buf.name, file_name);
	//����inode�ļ�����
	load_inode_entry(dir_buf.inode);
	((unsigned char *)(&(inode_buf.i_mode)))[0] = file_type;
	char *extension = strchr(file_name, '.');
	if (!extension || !strcmp(extension, ".exe") || !strcmp(extension, ".bin") || !strcmp(extension, ".com"))
		((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000111;
	update_inode_entry(dir_buf.inode);
	//����Ŀ¼ʱ
	if (file_type == FT_DIR) {
		//������������
		load_group_desc();
		gdt.bg_used_dirs_count++;
		update_group_desc();
		//�������ݿ�
		unsigned short i = new_block();
		memset(Buffer, 0, sizeof(Buffer));
		dir_entry temp;
		//������ǰĿ¼��
		memset(&temp, 0, sizeof(dir_entry));
		temp.inode = dir_buf.inode;
		temp.rec_len = 8;
		temp.name_len = 1;
		temp.file_type = FT_DIR;
		strcpy(temp.name, ".");
		((dir_entry *)Buffer)[0] = temp;
		//������һ��Ŀ¼��
		memset(&temp, 0, sizeof(dir_entry));
		temp.inode = current_dir;
		temp.rec_len = 9;
		temp.name_len = 2;
		temp.file_type = FT_DIR;
		strcpy(temp.name, "..");
		((dir_entry *)(Buffer + 8))[0] = temp;
		//�������ݿ�
		update_block_entry(i);
		//����inode
		load_inode_entry(dir_buf.inode);
		inode_buf.i_blocks = 1;
		inode_buf.i_size = 17;
		inode_buf.i_block[0] = i;
		((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000110;
		update_inode_entry(dir_buf.inode);
	}
}

//�鿴�ļ��Ƿ��
unsigned short is_open(unsigned short inode_num) {
	for (int i = 0; i < FOPEN_TABLE_MAX; ++i)
		if (fopen_table[i] == inode_num)
			return 1;
	return 0;
}

//�����ļ����������ݿ�
/*
��дʵ�鱨��ʱ��ͻȻ��ʶ��֮ǰ��һ������
�Ұ�inode.i_blocks����Ϊ��8��ָ�����ݿ��ָ���ʹ�ø�����
����ʵ����Ӧ���ļ������ݿ���������Ը��������Ϣ���ɵ�ȷ��
ÿ�����ݿ龿����ͨ��ֱ���������Ǽ���������ʡ�
����Ӱ�쵽�ĺ�����access_file��add_file_block
*/
unsigned short access_file(unsigned short inode_num, unsigned short (*func)(unsigned short)) {
	//func��һ�����ݿ���в�����������ֵ����ʱ���˳����ļ��ķ���
	load_inode_entry(inode_num);
	unsigned short indirect_1 = 0, indirect_2 = 0;
	for (unsigned short i = 0; i < inode_buf.i_blocks; /**/) {
		if (i < 6) {
			//ֱ������
			load_block_entry(inode_buf.i_block[i]);
			unsigned short ret = func(inode_buf.i_block[i]);
			update_block_entry(inode_buf.i_block[i]);
			++i;
			if (ret)
				return ret;
		} else if (i == 6) {
			//һ���������
			load_block_entry(inode_buf.i_block[i]);
			unsigned short j;
			if (indirect_1 < BLOCK_SIZE / sizeof(unsigned short))
				j = ((unsigned short *)Buffer)[indirect_1];
			if (j == 0 || indirect_1 == BLOCK_SIZE / sizeof(unsigned short)) {
				++i;
				indirect_1 = 0;
				continue;
			}
			++indirect_1;
			load_block_entry(j);
			unsigned short ret = func(j);
			update_block_entry(j);
			if (ret)
				return ret;
		} else {
			//�����������
			load_block_entry(inode_buf.i_block[i]);
			unsigned short j, k;
			if (indirect_1 < BLOCK_SIZE / sizeof(unsigned short))
				j = ((unsigned short *)Buffer)[indirect_1];
			if (j == 0 || indirect_1 == BLOCK_SIZE / sizeof(unsigned short))
				break;
			load_block_entry(j);
			if (indirect_2 < BLOCK_SIZE / sizeof(unsigned short))
				k = ((unsigned short *)Buffer)[indirect_2];
			if (k == 0)
				break;
			if (indirect_2 == BLOCK_SIZE / sizeof(unsigned short)) {
				++indirect_1;
				indirect_2 = 0;
				continue;
			}
			++indirect_2;
			load_block_entry(k);
			unsigned short ret = func(k);
			update_block_entry(k);
			if (ret)
				return ret;
		}
	}
	return 0;
}

//�ڵ������ݿ��в��ҿ���λ�ò�д��Ŀ¼��
unsigned short search_free_dir_in_block(unsigned short block) {
	unsigned short current_pos = 0;
	//ȷ������ temp.inode == 0
	memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
	dir_entry temp = ((dir_entry *)Buffer)[0];
	do {
		unsigned short k = temp.rec_len - temp.name_len - 7;
		if (k >= dir_buf.rec_len) {
			temp.rec_len = temp.name_len + 7;
			((dir_entry *)(Buffer + current_pos))[0].rec_len = temp.rec_len;
			dir_buf.rec_len = k;
			((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].inode = dir_buf.inode;
			((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].rec_len = dir_buf.rec_len;
			((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].name_len = dir_buf.name_len;
			((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].file_type = dir_buf.file_type;
			strcpy(((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].name, dir_buf.name);
			return 1;
		}
		current_pos += temp.rec_len;
		temp = ((dir_entry *)(Buffer + current_pos))[0];
	} while (temp.inode);
	if (BLOCK_SIZE - current_pos > dir_buf.rec_len) {
		((dir_entry *)(Buffer + current_pos))[0] = dir_buf;
		return 1;
	}
	return 0;
}

//�ڵ������ݿ��в����ļ�
unsigned short search_in_block(unsigned short block) {
	unsigned short current_pos = 0;
	//ȷ������ dir_buf.inode == 0
	memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
	dir_buf = ((dir_entry *)Buffer)[0];
	do {
		if (!strcmp(dir_buf.name, search_file_name))
			return dir_buf.inode;
		current_pos += dir_buf.rec_len;
		dir_buf = ((dir_entry *)(Buffer + current_pos))[0];
	} while (dir_buf.inode);
	return 0;
}

//�ڵ�ǰĿ¼�²����ļ�
unsigned short search_file(char name[]) {
	strcpy(search_file_name, name);
	return access_file(current_dir, search_in_block);
}

//�ڵ������ݿ���ɾ��Ŀ¼��
unsigned short delete_in_block(unsigned short block) {
	unsigned short current_pos = 0, pre_pos = 0;
	//ȷ������ dir_buf.inode == 0
	memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
	dir_buf = ((dir_entry *)Buffer)[0];
	do {
		if (!strcmp(dir_buf.name, search_file_name)) {
			((dir_entry *)(Buffer + pre_pos))[0].rec_len += dir_buf.rec_len;
			return dir_buf.inode;
		}
		pre_pos = current_pos;
		current_pos += dir_buf.rec_len;
		dir_buf = ((dir_entry *)(Buffer + current_pos))[0];
	} while (dir_buf.inode);
	return 0;
}

//�ͷ��ļ����ݿ�
unsigned short free_file_block(unsigned short block) {
	free_block(block);
	return 0;
}

//����ļ��������ݿ������
unsigned short print_file(unsigned short block) {
	for (unsigned short i = 0; i < BLOCK_SIZE; ++i) {
		if (Buffer[i] == 0)
			return 1;
		putchar(Buffer[i]);
	}
	return 0;
}

//���Ŀ¼�������ݿ������
unsigned short print_dir(unsigned short block) {
	inode inode_temp = inode_buf;
	unsigned short current_pos = 0;
	//ȷ������ dir_buf.inode == 0
	memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
	dir_buf = ((dir_entry *)Buffer)[0];
	do {
		load_inode_entry(dir_buf.inode);
		printf("%-10s", dir_buf.name);
		switch (dir_buf.file_type) {
			case FT_DIR:
				printf("<DIR>   ");
				break;
			case FT_REG_FILE:
				printf("<FILE>  ");
				break;
			default:
				printf("unknown ");
		}
		switch (((unsigned char *)(&(inode_buf.i_mode)))[1]) {
			case 2:
				printf("__w__    ");
				break;
			case 4:
				printf("r____    ");
				break;
			case 6:
				printf("r_w__    ");
				break;
			case 7:
				printf("r_w_x    ");
				break;
			default:
				printf("error  ");
		}
		printf("%-12hu", inode_buf.i_size);
		time_t temp;
		char time_str[26];
		temp = inode_buf.i_ctime;
		strcpy(time_str, ctime(&temp));
		time_str[24] = '\0';
		printf("%s  ", time_str);
		temp = inode_buf.i_atime;
		strcpy(time_str, ctime(&temp));
		time_str[24] = '\0';
		printf("%s  ", time_str);
		temp = inode_buf.i_mtime;
		strcpy(time_str, ctime(&temp));
		time_str[24] = '\0';
		printf("%s  \n", time_str);
		current_pos += dir_buf.rec_len;
		dir_buf = ((dir_entry *)(Buffer + current_pos))[0];
	} while (dir_buf.inode);
	inode_buf = inode_temp;
	return 0;
}

//Ϊ�ļ��������ݿ�
unsigned short add_file_block(unsigned short inode_num) {
	unsigned short i = 0;
	load_inode_entry(inode_num);
	if (inode_buf.i_blocks < 6) {
		i = new_block();
		inode_buf.i_blocks++;
		inode_buf.i_block[inode_buf.i_blocks - 1] = i;
	} else if (inode_buf.i_blocks == 6) {
		i = new_block();
		inode_buf.i_blocks++;
		inode_buf.i_block[inode_buf.i_blocks - 1] = i;
		i = new_block();
		memset(Buffer, 0, sizeof(Buffer));
		((unsigned short *)Buffer)[0] = i;
		update_block_entry(inode_buf.i_block[inode_buf.i_blocks - 1]);
	} else if (inode_buf.i_blocks == 7) {
		load_block_entry(inode_buf.i_block[6]);
		unsigned short j;
		for (j = 0; j < 256 && ((unsigned short *)Buffer)[j]; ++j);
		if (j == 256) {
			i = new_block();
			inode_buf.i_blocks++;
			inode_buf.i_block[inode_buf.i_blocks - 1] = i;
			i = new_block();
			memset(Buffer, 0, sizeof(Buffer));
			((unsigned short *)Buffer)[0] = i;
			update_block_entry(inode_buf.i_block[inode_buf.i_blocks - 1]);
			unsigned short k = new_block();
			memset(Buffer, 0, sizeof(Buffer));
			((unsigned short *)Buffer)[0] = k;
			update_block_entry(i);
			i = k;
		} else {
			i = new_block();
			((unsigned short *)Buffer)[j] = i;
		}
		update_block_entry(inode_buf.i_block[6]);
	} else {
		load_block_entry(inode_buf.i_block[7]);
		unsigned short j, k;
		for (j = 0; j < 256 && ((unsigned short *)Buffer)[j]; ++j);
		--j;
		load_block_entry(j);
		for (k = 0; k < 256 && ((unsigned short *)Buffer)[k]; ++k);
		if (k < 256) {
			i = new_block();
			((unsigned short *)Buffer)[k] = i;
			update_block_entry(j);
		} else {
			if (j < 255) {
				load_block_entry(inode_buf.i_block[7]);
				++j;
				unsigned short temp = new_block();
				((unsigned short *)Buffer)[j] = temp;
				update_block_entry(inode_buf.i_block[7]);
				memset(Buffer, 0, sizeof(Buffer));
				i = new_block();
				((unsigned short *)Buffer)[0] = i;
				update_block_entry(temp);
			} else
				printf("The file has reached the maximum capacity!\n");
		}
	}
	update_inode_entry(inode_num);
	return i;
}

//gets_s()������C11�ı�������չ������滻����ȫ��gets()
//��Linux��gcc�в���֧�ָú���
//��ʹ��fgets()��ʵ��ָ�������ַ��Ķ�ȡ����Ҳ�����β�Ļ��з�����
//�����Ҫ�ÿ��ַ����滻���з�
char *gets_s(char *buffer, int num) {
	if (fgets(buffer, num, stdin) != 0) {
		size_t len = strlen(buffer);
		if (len > 0 && buffer[len - 1] == '\n')
			buffer[len - 1] = '\0';
		return buffer;
	}
	return NULL;
}


/**********************��ʼ���ļ�ϵͳ**********************/

//��ʼ���ڴ�����
void initialize_memory(void) {
	//����ļ��򿪱�
	memset(fopen_table, 0, sizeof(fopen_table));
	//��ʼ����������
	strcpy(gdt.bg_volume_name, VOLUME_NAME);
	strcpy(gdt.bg_password, "666666");
	gdt.bg_block_bitmap = 1;
	gdt.bg_inode_bitmap = 2;
	gdt.bg_inode_table = 3;
	gdt.bg_free_blocks_count = DATA_BLOCK_COUNT;
	gdt.bg_free_inodes_count = INODE_COUNT;
	gdt.bg_used_dirs_count = 0;
	//��ʼ����ǰ·��
	current_dir = 1;
	strcpy(current_path, "root");
}

//��ʼ������
void initialize_disk(void) {
	//��մ���
	memset(Buffer, 0, sizeof(Buffer));
	for (int i = 0; i < DISK_SIZE; ++i)
		disk_IO(i * BLOCK_SIZE, Buffer, WRITE_DISK);
	//����������д�����
	update_group_desc();
	//������Ŀ¼
	new_dir_entry("root", FT_DIR);
}


/************************�����************************/

//��¼
unsigned short login(char password[]) {
	load_group_desc();
	return !(strcmp(gdt.bg_password, password));
}

//�޸�����
void change_password(void) {
	printf("Old password: ");
	char password[10];
	gets_s(password, 9);
	if (login(password)) {
		printf("New password(no more than 9): ");
		gets_s(password, 9);
		char confirm[10];
		printf("Confirm password: ");
		gets_s(confirm, 9);
		if (!strcmp(password, confirm)) {
			load_group_desc();
			strcpy(gdt.bg_password, password);
			update_group_desc();
			printf("The password is changed.\n");
		} else
			printf("Please try again.\n");
	} else
		printf("Password error!\n");
}

//�г���ǰĿ¼
void dir(void) {
	printf("name      ");
	printf("type    ");
	printf("mode    ");
	printf("size(Byte)   ");
	printf("creat time                ");
	printf("access time               ");
	printf("modify time               \n");
	access_file(current_dir, print_dir);
	load_inode_entry(current_dir);
	inode_buf.i_atime = time(NULL);
	update_inode_entry(current_dir);
}

//����Ŀ¼
void mkdir(char name[]) {
	unsigned short i = search_file(name);
	if (i) {
		printf("A directory with the same name exists.\n");
	} else {
		new_dir_entry(name, FT_DIR);
		//�ڵ�ǰĿ¼������½�Ŀ¼��
		if (!access_file(current_dir, search_free_dir_in_block)) {
			//���ݿ鲻��ʱ
			unsigned short j = add_file_block(i);
			memset(Buffer, 0, sizeof(Buffer));
			((dir_entry *)Buffer)[0] = dir_buf;
			update_block_entry(j);
		}
		load_inode_entry(current_dir);
		inode_buf.i_size += (7 + strlen(name));
		update_inode_entry(current_dir);
	}
}

//ɾ����Ŀ¼
void rmdir(char name[]) {
	if ((!strcmp(name, ".")) || (!strcmp(name, ".."))) {
		printf("Wrong command!\n");
		return;
	}
	unsigned short i = search_file(name);
	if (i) {
		load_inode_entry(i);
		if (((unsigned char *)(&(inode_buf.i_mode)))[0] != FT_DIR) {
			printf("Wrong command!\n");
			return;
		}
		if (inode_buf.i_size != 17) {
			printf("Cannot delete non empty directory!\n");
			return;
		}
		//ɾ����ǰĿ¼�µ�Ŀ¼��
		access_file(current_dir, delete_in_block);
		//�ͷ�inode�����ݿ�
		access_file(i, free_file_block);
		load_inode_entry(i);
		inode_buf.i_dtime = time(NULL);
		update_inode_entry(i);
		free_inode(i);
		//���µ�ǰĿ¼��С
		load_inode_entry(current_dir);
		inode_buf.i_size -= (7 + strlen(name));
		update_inode_entry(current_dir);
		//������������
		load_group_desc();
		gdt.bg_used_dirs_count--;
		update_group_desc();
	} else {
		printf("The directory does not exist.\n");
	}
}

//�����ļ�
void create(char name[]) {
	unsigned short i = search_file(name);
	if (i) {
		printf("A file with the same name exists.\n");
	} else {
		new_dir_entry(name, FT_REG_FILE);
		//�ڵ�ǰĿ¼������½�Ŀ¼��
		if (!access_file(current_dir, search_free_dir_in_block)) {
			//���ݿ鲻��ʱ
			unsigned short j = add_file_block(i);
			memset(Buffer, 0, sizeof(Buffer));
			((dir_entry *)Buffer)[0] = dir_buf;
			update_block_entry(j);
		}
		load_inode_entry(current_dir);
		inode_buf.i_size += (7 + strlen(name));
		update_inode_entry(current_dir);
	}
}

//ɾ���ļ�
void delete (char name[]) {
	unsigned short i = search_file(name);
	if (i) {
		load_inode_entry(i);
		if (((unsigned char *)(&(inode_buf.i_mode)))[0] != FT_REG_FILE) {
			printf("Wrong command!\n");
			return;
		}
		if (is_open(i))
			printf("The file is in use! Please close it first.\n");
		else {
			//ɾ����ǰĿ¼�µ�Ŀ¼��
			access_file(current_dir, delete_in_block);
			//�ͷ�inode�����ݿ�
			access_file(i, free_file_block);
			load_inode_entry(i);
			inode_buf.i_dtime = time(NULL);
			update_inode_entry(i);
			free_inode(i);
			//���µ�ǰĿ¼��С
			load_inode_entry(current_dir);
			inode_buf.i_size -= (7 + strlen(name));
			update_inode_entry(current_dir);
		}
	} else {
		printf("The file does not exist.\n");
	}
}

//�л�����Ŀ¼
void cd(char path[]) {
	if (!strcmp(path, "") || !strcmp(path, "~")) {
		current_dir = 1;
		strcpy(current_path, "root");
	} else if (!strcmp(path, ".")) {
		;	//do nothing
	} else if (!strcmp(path, "..")) {
		load_inode_entry(current_dir);
		load_block_entry(inode_buf.i_block[0]);
		current_dir = ((dir_entry *)(Buffer + 8))[0].inode;
		for (int k = strlen(current_path); k >= 0; --k) {
			if (current_path[k] == '/') {
				current_path[k] = '\0';
				break;
			}
		}
	} else {
		unsigned short i = search_file(path);
		if (i) {
			load_inode_entry(i);
			if (((unsigned char *)(&(inode_buf.i_mode)))[0] != FT_DIR) {
				printf("No such directory exits!\n");
				return;
			}
			current_dir = i;
			strcat(current_path, "/");
			strcat(current_path, path);
		} else {
			printf("No such directory exits!\n");
		}
	}
}

//�����ļ�������
void attrib(char name[], unsigned char change) {
	unsigned short i = search_file(name);
	if (i == 0)
		printf("The file does not exist.\n");
	else {
		if (change == 2 || change == 4 || change == 6 || change == 7) {
			load_inode_entry(i);
			((unsigned char *)(&(inode_buf.i_mode)))[1] = change;
			inode_buf.i_atime = time(NULL);
			inode_buf.i_mtime = time(NULL);
			update_inode_entry(i);
		} else
			printf("Wrong modification!\n");
	}
}

//���ļ�
void open(char name[]) {
	unsigned short inode_num = search_file(name);
	if (inode_num == 0)
		printf("The file does not exist.\n");
	else {
		if (is_open(inode_num))
			printf("The file has opened.\n");
		else {
			load_inode_entry(inode_num);
			unsigned char permission = ((unsigned char *)(&(inode_buf.i_mode)))[1];
			if (permission == 4 || permission == 6 || permission == 7) {
				inode_buf.i_atime = time(NULL);
				update_inode_entry(inode_num);
				for (unsigned short i = 0; i < FOPEN_TABLE_MAX; ++i) {
					if (fopen_table[i] == 0) {
						fopen_table[i] = inode_num;
						return;
					}
				}
				printf("The number of files opened has reached the maximum.\n");
			} else
				printf("You do not have permission to open this file.\n");
		}
	}
}

//�ر��ļ�
void close(char name[]) {
	unsigned short inode_num = search_file(name);
	if (inode_num == 0)
		printf("The file does not exist.\n");
	else {
		if (is_open(inode_num)) {
			load_inode_entry(inode_num);
			inode_buf.i_atime = time(NULL);
			update_inode_entry(inode_num);
			for (unsigned short i = 0; i < FOPEN_TABLE_MAX; ++i) {
				if (fopen_table[i] == inode_num) {
					fopen_table[i] = 0;
					return;
				}
			}
		} else
			printf("The file does not open.\n");
	}
}

//���ļ�
void read(char name[]) {
	unsigned short i = search_file(name);
	if (i == 0)
		printf("The file does not exist.\n");
	else {
		load_inode_entry(i);
		unsigned char permission = ((unsigned char *)(&(inode_buf.i_mode)))[1];
		if (permission == 4 || permission == 6 || permission == 7) {
			if (is_open(i)) {
				access_file(i, print_file);
				inode_buf.i_atime = time(NULL);
				update_inode_entry(i);
			} else
				printf("The file does not open.\n");
		} else
			printf("You do not have permission to read this file.\n");
	}
}

//д�ļ����Ը��ӷ�ʽ��
int flag = 1;		//�����˳�д����
void stopWrite() { flag = 0; }
void write(char name[]) {
	unsigned short i = search_file(name);
	if (i == 0)
		printf("The file does not exist.\n");
	else {
		load_inode_entry(i);
		unsigned char permission = ((unsigned char *)(&(inode_buf.i_mode)))[1];
		if (permission == 2 || permission == 6 || permission == 7) {
			if (is_open(i)) {
				//��λ�ļ�ĩβ
				unsigned short pos = 0, j = 0;
				if (inode_buf.i_blocks == 0) {
					j = new_block();
					inode_buf.i_blocks = 1;
					inode_buf.i_block[0] = j;
					update_inode_entry(i);
					load_block_entry(j);
					pos = 0;
				} else if (inode_buf.i_blocks <= 6) {
					j = inode_buf.i_block[inode_buf.i_blocks - 1];
					load_block_entry(j);
					pos = inode_buf.i_size % BLOCK_SIZE;
					if (pos == 0) {
						j = add_file_block(i);
						load_block_entry(j);
					}
				} else if (inode_buf.i_blocks == 7) {
					load_block_entry(inode_buf.i_block[6]);
					pos = inode_buf.i_size / BLOCK_SIZE - 6;
					j = ((unsigned short *)Buffer)[pos];
					load_block_entry(j);
					pos = inode_buf.i_size % BLOCK_SIZE;
					if (pos == 0) {
						j = add_file_block(i);
						load_block_entry(j);
					}
				} else {
					load_block_entry(inode_buf.i_block[7]);
					pos = (inode_buf.i_size / BLOCK_SIZE - 6 - 256) / 256;
					load_block_entry(((unsigned short *)Buffer)[pos]);
					pos = (inode_buf.i_size / BLOCK_SIZE - 6 - 256) % 256;
					j = ((unsigned short *)Buffer)[pos];
					load_block_entry(j);
					pos = inode_buf.i_size % BLOCK_SIZE;
					if (pos == 0) {
						j = add_file_block(i);
						load_block_entry(j);
					}
				}
				//д�����ݿ�
				unsigned short new_size = 0;
				char ch = getchar();
				//��Windows�п�ʹ��EOF��crtl+z������Ϊ�ļ��������Ľ���
				//����Linux��ʹ��EOF��ctrl+d��ʱ�����������벻���Ĵ���
				//��Linux������EOF�ᵼ�º������������뺯��ʧЧ
				//��ˣ���Linux�¿�ͨ�����ж���ʵ��д�����Ľ���
				//ͨ������SIGQUIT��ctrl+\���źŵĴ��������˳�д����ѭ��
				signal(SIGQUIT, stopWrite);
				while (flag/* ch != EOF */) {
					Buffer[pos] = ch;
					pos++;
					new_size++;
					ch = getchar();
					if (pos == BLOCK_SIZE) {
						update_block_entry(j);
						j = add_file_block(i);
						load_block_entry(j);
						pos = 0;
					}
				}
				flag = 1;
				Buffer[pos] = '\0';
				update_block_entry(j);
				inode_buf.i_size += new_size - 1;
				inode_buf.i_atime = time(NULL);
				inode_buf.i_mtime = time(NULL);
				update_inode_entry(i);
			} else
				printf("The file does not open.\n");
		} else
			printf("You do not have permission to write this file.\n");
	}
}

//��ʾ������Ϣ
void check_disk(void) {
	load_group_desc();
	printf("Volume Name: %s\n", gdt.bg_volume_name);
	printf("Block Size: %dBytes\n", BLOCK_SIZE);
	printf("Free Block: %u\n", gdt.bg_free_blocks_count);
	printf("Free Inode: %u\n", gdt.bg_free_inodes_count);
	printf("Directories: %u\n", gdt.bg_used_dirs_count);
}

//��ʽ��
void format(void) {
	initialize_memory();
	initialize_disk();
	printf("Format succeeded!\n");
	check_disk();
}


/***********************�û��ӿڲ�***********************/

//�û��ӿ�
void shell(void) {
	char cmd[256] = "";
	while (1) {
		printf("[%s]# ", current_path);
		gets_s(cmd, 256);
		if (!strcmp(cmd, "format")) {
			format();
		} else if (!strcmp(cmd, "check")) {
			check_disk();
		} else if (!strcmp(cmd, "password")) {
			change_password();
		} else if (!strcmp(cmd, "ls")) {
			dir();
		} else if (!strncmp(cmd, "mkdir ", 6)) {
			mkdir(cmd + 6);
		} else if (!strncmp(cmd, "rmdir ", 6)) {
			rmdir(cmd + 6);
		} else if (!strncmp(cmd, "create ", 7)) {
			create(cmd + 7);
		} else if (!strncmp(cmd, "delete ", 7)) {
			delete (cmd + 7);
		} else if (!strncmp(cmd, "cd ", 3)) {
			cd(cmd + 3);
		} else if (!strncmp(cmd, "chmod ", 6)) {
			printf("modification: ");
			unsigned char change;
			scanf("%hhu", &change);
			getchar();
			attrib(cmd + 6, change);
		} else if (!strncmp(cmd, "open ", 5)) {
			open(cmd + 5);
		} else if (!strncmp(cmd, "close ", 6)) {
			close(cmd + 6);
		} else if (!strncmp(cmd, "read ", 5)) {
			read(cmd + 5);
		} else if (!strncmp(cmd, "write ", 6)) {
			write(cmd + 6);
		} else if (!strcmp(cmd, "quit")) {
			break;
		} else {
			printf("Wrong command!\n");
		}
	}
}

int main(void) {
	fp = fopen("./Ext2", "rb+");
	initialize_memory();
	if (fp == NULL) {
		fp = fopen("./Ext2", "wb+");
		initialize_disk();
	}
	printf("Password: ");
	char password[10];
	gets_s(password, 9);
	while (!login(password)) {
		printf("Error!\n");
		printf("Password: ");
		gets_s(password, 9);
	}
	printf("************************************\n");
	printf("    Welcome to EXT2 file system!\n");
	printf("************************************\n");
	shell();
	fclose(fp);

	return 0;
}