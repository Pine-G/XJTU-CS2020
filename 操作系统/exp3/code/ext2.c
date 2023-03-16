//类EXT2文件系统：逻辑块与物理块大小均为512B，假设只有一个用户，只定义一个组，省略超级块

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define VOLUME_NAME "EXT2FS"                //卷名
#define EXT2_NAME_LEN 255                   //文件名最大长度
#define FOPEN_TABLE_MAX 16                  //文件打开表最大大小
#define BLOCK_SIZE 512                      //数据块大小
#define DATA_BLOCK_COUNT 4096               //数据块个数
#define INODE_COUNT 4096                    //inode个数
#define DISK_SIZE 4611                      //磁盘块个数
#define READ_DISK 1                         //读磁盘
#define WRITE_DISK 0                        //写磁盘
#define GDT_START 0                         //组描述符起始偏移
#define BLOCK_BITMAP_START 512              //数据块位图起始偏移
#define INODE_BITMAP_START 1024             //inode位图起始偏移
#define INODE_TABLE_START 1536              //inode表起始偏移
#define DATA_BLOCK_START (1536+512*512)     //数据块起始偏移

//文件类型
enum {
    FT_UNKNOWN,                             //未知
    FT_REG_FILE,                            //普通文件
    FT_DIR,                                 //目录
    FT_CHRDEV,                              //字符设备
    FT_BLKDEV,                              //块设备
    FT_FIFO,                                //管道
    FT_SOCK,                                //套接字
    FT_SYMLINK,                             //符号指针
};

//组描述符(32B)
typedef struct group_desc {
    char bg_volume_name[10];                //卷名
    unsigned short bg_block_bitmap;         //保存数据块位图的块号
    unsigned short bg_inode_bitmap;         //保存inode位图的块号
    unsigned short bg_inode_table;          //inode表的起始块号
    unsigned short bg_free_blocks_count;    //本组空闲块个数
    unsigned short bg_free_inodes_count;    //本组空闲inode个数
    unsigned short bg_used_dirs_count;      //本组目录个数
    char bg_password[10];                   //登录密码
} group_desc;

//索引结点(64B)
typedef struct inode {
    unsigned short i_mode;                  //文件类型及访问权限
    unsigned short i_blocks;                //文件的数据块个数
    unsigned int i_size;                    //文件或目录的大小（字节）
    unsigned int i_atime;                   //访问时间
    unsigned int i_ctime;                   //创建时间
    unsigned int i_mtime;                   //修改时间
    unsigned int i_dtime;                   //删除时间
    unsigned short i_block[8];              //指向数据块的指针
    char i_pad[24];                         //填充（0xff）
} inode;

//目录项（变长7-261B）
typedef struct dir_entry {
    unsigned short inode;                   //索引结点号
    unsigned short rec_len;                 //目录项长度
    unsigned char name_len;                 //文件名长度
    unsigned char file_type;                //文件类型
    char name[EXT2_NAME_LEN];               //文件名
} dir_entry;

//读写缓冲区
group_desc gdt;                             //组描述符缓冲区
inode inode_buf;                            //索引结点缓冲区
dir_entry dir_buf;                          //目录项缓冲区
unsigned char block_bitmap[BLOCK_SIZE];     //块位图缓冲区
unsigned char inode_bitmap[BLOCK_SIZE];     //inode位图缓冲区
unsigned char gdt_Buffer[BLOCK_SIZE];       //组描述符读写缓冲区
unsigned char inode_Buffer[BLOCK_SIZE];     //索引结点数据块缓冲区
unsigned char Buffer[BLOCK_SIZE * 2];       //数据块缓冲区

/* Buffer的后半部分并不存储实际数据，只是用来防止写入数据时溢出 */

FILE *fp = NULL;                            //虚拟磁盘
char current_path[256];                     //当前路径（字符串）
unsigned short current_dir;                 //当前目录（索引结点）
unsigned short fopen_table[FOPEN_TABLE_MAX];//文件打开表
char search_file_name[EXT2_NAME_LEN];       //待查找的文件名


/************************IO操作************************/

//磁盘I/O（以块为单位）
void disk_IO(long offset, void *buf, int rw_flag) {
    fseek(fp, offset, SEEK_SET);
    if (rw_flag) {
        fread(buf, BLOCK_SIZE, 1, fp);
    } else {
        fwrite(buf, BLOCK_SIZE, 1, fp);
        fflush(fp);
    }
}

//载入组描述符
void load_group_desc(void) {
    disk_IO(GDT_START, gdt_Buffer, READ_DISK);
    gdt = ((group_desc *)gdt_Buffer)[0];
}

//更新组描述符
void update_group_desc(void) {
    memset(gdt_Buffer, 0xff, sizeof(gdt_Buffer));
    ((group_desc *)gdt_Buffer)[0] = gdt;
    disk_IO(GDT_START, gdt_Buffer, WRITE_DISK);
}

/* inode从1开始计数，0表示NULL；数据块从0开始计数 */

//载入第k个inode
void load_inode_entry(unsigned short k) {
    --k;
    unsigned short i = k / 8, j = k % 8;
    disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, READ_DISK);
    inode_buf = ((inode *)inode_Buffer)[j];
}

//更新第k个inode
void update_inode_entry(unsigned short k) {
    --k;
    unsigned short i = k / 8, j = k % 8;
    disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, READ_DISK);
    ((inode *)inode_Buffer)[j] = inode_buf;
    disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, WRITE_DISK);
}

//载入第i个数据块
void load_block_entry(unsigned short i) {
    disk_IO(DATA_BLOCK_START + i * BLOCK_SIZE, Buffer, READ_DISK);
}

//更新第i个数据块
void update_block_entry(unsigned short i) {
    disk_IO(DATA_BLOCK_START + i * BLOCK_SIZE, Buffer, WRITE_DISK);
}

//载入数据块位图
void load_block_bitmap(void) {
    disk_IO(BLOCK_BITMAP_START, block_bitmap, READ_DISK);
}

//更新数据块位图
void update_block_bitmap(void) {
    disk_IO(BLOCK_BITMAP_START, block_bitmap, WRITE_DISK);
}

//载入inode位图
void load_inode_bitmap(void) {
    disk_IO(INODE_BITMAP_START, inode_bitmap, READ_DISK);
}

//更新inode位图
void update_inode_bitmap(void) {
    disk_IO(INODE_BITMAP_START, inode_bitmap, WRITE_DISK);
}


/*************************底层*************************/

//返回位图中的第一个0的位置
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

//将位图中的第k位取反
void bitmap_neg_k(unsigned char bitmap[], int k) {
    int i = k / 8, j = k % 8;
    unsigned char mask = 0b10000000;
    for (int t = 0; t < j; ++t)
        mask >>= 1;
    bitmap[i] ^= mask;
}

//初始化inode
void initialize_inode(void) {
    inode_buf.i_mode = FT_UNKNOWN;
    inode_buf.i_mode <<= 8;
    //默认的访问权限：可读可写不可执行
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

//分配inode
unsigned short new_inode(void) {
    load_group_desc();
    if (gdt.bg_free_inodes_count) {
        //更新inode位图
        load_inode_bitmap();
        unsigned short i = bitmap_find_0(inode_bitmap);
        bitmap_neg_k(inode_bitmap, i);
        update_inode_bitmap();
        //更新inode表
        initialize_inode();
        update_inode_entry(i + 1);
        //更新组描述符
        gdt.bg_free_inodes_count--;
        update_group_desc();
        return i + 1;
    } else {
        printf("There is no inode to be allocated!\n");
        return 0;
    }
}

//释放inode
void free_inode(unsigned short i) {
    //更新inode位图
    load_inode_bitmap();
    bitmap_neg_k(inode_bitmap, i - 1);
    update_inode_bitmap();
    //更新组描述符
    load_group_desc();
    gdt.bg_free_inodes_count++;
    update_group_desc();
}

//分配数据块
unsigned short new_block(void) {
    load_group_desc();
    if (gdt.bg_free_blocks_count) {
        //更新数据块位图
        load_block_bitmap();
        unsigned short i = bitmap_find_0(block_bitmap);
        bitmap_neg_k(block_bitmap, i);
        update_block_bitmap();
        //更新组描述符
        gdt.bg_free_blocks_count--;
        update_group_desc();
        return i;
    } else {
        printf("There is no block to be allocated!\n");
        return 0;
    }
}

//释放数据块
void free_block(unsigned short i) {
    //更新数据块位图
    load_block_bitmap();
    bitmap_neg_k(block_bitmap, i);
    update_block_bitmap();
    //更新组描述符
    load_group_desc();
    gdt.bg_free_blocks_count++;
    update_group_desc();
}

//创建目录项
void new_dir_entry(char file_name[], unsigned char file_type) {
    memset(&dir_buf, 0, sizeof(dir_entry));
    dir_buf.inode = new_inode();
    dir_buf.name_len = strlen(file_name);
    dir_buf.rec_len = 7 + dir_buf.name_len;
    dir_buf.file_type = file_type;
    strcpy(dir_buf.name, file_name);
    //更新inode文件类型
    load_inode_entry(dir_buf.inode);
    ((unsigned char *)(&(inode_buf.i_mode)))[0] = file_type;
    char *extension = strchr(file_name, '.');
    if (!extension || !strcmp(extension, ".exe") || !strcmp(extension, ".bin") || !strcmp(extension, ".com"))
        ((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000111;
    update_inode_entry(dir_buf.inode);
    //建立目录时
    if (file_type == FT_DIR) {
        //更新组描述符
        load_group_desc();
        gdt.bg_used_dirs_count++;
        update_group_desc();
        //分配数据块
        unsigned short i = new_block();
        memset(Buffer, 0, sizeof(Buffer));
        dir_entry temp;
        //创建当前目录项
        memset(&temp, 0, sizeof(dir_entry));
        temp.inode = dir_buf.inode;
        temp.rec_len = 8;
        temp.name_len = 1;
        temp.file_type = FT_DIR;
        strcpy(temp.name, ".");
        ((dir_entry *)Buffer)[0] = temp;
        //创建上一级目录项
        memset(&temp, 0, sizeof(dir_entry));
        temp.inode = current_dir;
        temp.rec_len = 9;
        temp.name_len = 2;
        temp.file_type = FT_DIR;
        strcpy(temp.name, "..");
        ((dir_entry *)(Buffer + 8))[0] = temp;
        //更新数据块
        update_block_entry(i);
        //更新inode
        load_inode_entry(dir_buf.inode);
        inode_buf.i_blocks = 1;
        inode_buf.i_size = 17;
        inode_buf.i_block[0] = i;
        ((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000110;
        update_inode_entry(dir_buf.inode);
    }
}

//查看文件是否打开
unsigned short is_open(unsigned short inode_num) {
    for (int i = 0; i < FOPEN_TABLE_MAX; ++i)
        if (fopen_table[i] == inode_num)
            return 1;
    return 0;
}

//访问文件的所有数据块
//已验证两级索引没有问题，三级索引尚不清楚
/*
在写实验报告时，突然意识到之前的一个错误。
我把inode.i_blocks误认为是8个指向数据块的指针的使用个数，
但它实际上应是文件的数据块个数，可以根据这个信息轻松地确定
每个数据块究竟是通过直接索引还是间接索引访问。
具体影响到的函数有access_file、add_file_block
*/
unsigned short access_file(unsigned short inode_num, unsigned short (*func)(unsigned short)) {
    //func对一个数据块进行操作，当返回值非零时，退出对文件的访问
    load_inode_entry(inode_num);
    unsigned short indirect_1 = 0, indirect_2 = 0;
    for (unsigned short i = 0; i < inode_buf.i_blocks; /**/) {
        if (i < 6) {
            //直接索引
            load_block_entry(inode_buf.i_block[i]);
            unsigned short ret = func(inode_buf.i_block[i]);
            update_block_entry(inode_buf.i_block[i]);
            ++i;
            if (ret)
                return ret;
        } else if (i == 6) {
            //一级间接索引
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
            //二级间接索引
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

//在单个数据块中查找空闲位置并写入目录项
unsigned short search_free_dir_in_block(unsigned short block) {
    unsigned short current_pos = 0;
    //确保存在 temp.inode == 0
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

//在单个数据块中查找文件
unsigned short search_in_block(unsigned short block) {
    unsigned short current_pos = 0;
    //确保存在 dir_buf.inode == 0
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

//在当前目录下查找文件
unsigned short search_file(char name[]) {
    strcpy(search_file_name, name);
    return access_file(current_dir, search_in_block);
}

//在单个数据块中删除目录项
unsigned short delete_in_block(unsigned short block) {
    unsigned short current_pos = 0, pre_pos = 0;
    //确保存在 dir_buf.inode == 0
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

//释放文件数据块
unsigned short free_file_block(unsigned short block) {
    free_block(block);
    return 0;
}

//输出文件单个数据块的内容
unsigned short print_file(unsigned short block) {
    for (unsigned short i = 0; i < BLOCK_SIZE; ++i) {
        if (Buffer[i] == 0)
            return 1;
        putchar(Buffer[i]);
    }
    return 0;
}

//输出目录单个数据块的内容
unsigned short print_dir(unsigned short block) {
    inode inode_temp = inode_buf;
    unsigned short current_pos = 0;
    //确保存在 dir_buf.inode == 0
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

//为文件新增数据块
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

//gets_s()函数是C11的编译器扩展项，用于替换不安全的gets()
//在Linux的gcc中并不支持该函数
//可使用fgets()来实现指定个数字符的读取，但也会把行尾的换行符读入
//因此需要用空字符来替换换行符
char *gets_s(char *buffer, int num) {
    if (fgets(buffer, num, stdin) != 0) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        return buffer;
    }
    return NULL;
}


/**********************初始化文件系统**********************/

//初始化内存数据
void initialize_memory(void) {
    //清空文件打开表
    memset(fopen_table, 0, sizeof(fopen_table));
    //初始化组描述符
    strcpy(gdt.bg_volume_name, VOLUME_NAME);
    strcpy(gdt.bg_password, "666666");
    gdt.bg_block_bitmap = 1;
    gdt.bg_inode_bitmap = 2;
    gdt.bg_inode_table = 3;
    gdt.bg_free_blocks_count = DATA_BLOCK_COUNT;
    gdt.bg_free_inodes_count = INODE_COUNT;
    gdt.bg_used_dirs_count = 0;
    //初始化当前路径
    current_dir = 1;
    strcpy(current_path, "root");
}

//初始化磁盘
void initialize_disk(void) {
    //清空磁盘
    memset(Buffer, 0, sizeof(Buffer));
    for (int i = 0; i < DISK_SIZE; ++i)
        disk_IO(i * BLOCK_SIZE, Buffer, WRITE_DISK);
    //将组描述符写入磁盘
    update_group_desc();
    //创建根目录
    new_dir_entry("root", FT_DIR);
}


/************************命令层************************/

//登录
unsigned short login(char password[]) {
    load_group_desc();
    return !(strcmp(gdt.bg_password, password));
}

//修改密码
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

//列出当前目录
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

//建立目录
void mkdir(char name[]) {
    unsigned short i = search_file(name);
    if (i) {
        printf("A directory with the same name exists.\n");
    } else {
        new_dir_entry(name, FT_DIR);
        //在当前目录下添加新建目录项
        if (!access_file(current_dir, search_free_dir_in_block)) {
            //数据块不够时
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

//删除空目录
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
        //删除当前目录下的目录项
        access_file(current_dir, delete_in_block);
        //释放inode及数据块
        access_file(i, free_file_block);
        load_inode_entry(i);
        inode_buf.i_dtime = time(NULL);
        update_inode_entry(i);
        free_inode(i);
        //更新当前目录大小
        load_inode_entry(current_dir);
        inode_buf.i_size -= (7 + strlen(name));
        update_inode_entry(current_dir);
        //更新组描述符
        load_group_desc();
        gdt.bg_used_dirs_count--;
        update_group_desc();
    } else {
        printf("The directory does not exist.\n");
    }
}

//建立文件
void create(char name[]) {
    unsigned short i = search_file(name);
    if (i) {
        printf("A file with the same name exists.\n");
    } else {
        new_dir_entry(name, FT_REG_FILE);
        //在当前目录下添加新建目录项
        if (!access_file(current_dir, search_free_dir_in_block)) {
            //数据块不够时
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

//删除文件
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
            //删除当前目录下的目录项
            access_file(current_dir, delete_in_block);
            //释放inode及数据块
            access_file(i, free_file_block);
            load_inode_entry(i);
            inode_buf.i_dtime = time(NULL);
            update_inode_entry(i);
            free_inode(i);
            //更新当前目录大小
            load_inode_entry(current_dir);
            inode_buf.i_size -= (7 + strlen(name));
            update_inode_entry(current_dir);
        }
    } else {
        printf("The file does not exist.\n");
    }
}

//切换单级目录
void cd(char path[]) {
    if (!strcmp(path, "") || !strcmp(path, "~")) {
        current_dir = 1;
        strcpy(current_path, "root");
    } else if (!strcmp(path, ".")) {
        ;   //do nothing
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

//更改文件保护码
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

//打开文件
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

//关闭文件
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

//读文件
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

//写文件（以附加方式）
int flag = 1;       //用于退出写操作
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
                //定位文件末尾
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
                //写入数据块
                unsigned short new_size = 0;
                char ch = getchar();
                //在Windows中可使用EOF（crtl+z）来作为文件输入流的结束
                //但在Linux中使用EOF（ctrl+d）时，则会出现意想不到的错误
                //在Linux中输入EOF会导致后续的所有输入函数失效
                //因此，在Linux下可通过软中断来实现写操作的结束
                //通过定义SIGQUIT（ctrl+\）信号的处理函数来退出写操作循环
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

//显示磁盘信息
void check_disk(void) {
    load_group_desc();
    printf("Volume Name: %s\n", gdt.bg_volume_name);
    printf("Block Size: %dBytes\n", BLOCK_SIZE);
    printf("Free Block: %u\n", gdt.bg_free_blocks_count);
    printf("Free Inode: %u\n", gdt.bg_free_inodes_count);
    printf("Directories: %u\n", gdt.bg_used_dirs_count);
}

//格式化
void format(void) {
    initialize_memory();
    initialize_disk();
    printf("Format succeeded!\n");
    check_disk();
}


/***********************用户接口层***********************/

//用户接口
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
