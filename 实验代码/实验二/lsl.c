#include<stdio.h>
#include<grp.h>
#include<time.h>
#include<pwd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
struct stat sbuf;
struct stat sbuf_dst;
struct stat sbuf_reg;
void print_information(char* file);

void open_dir(char* dir)
{
    DIR *pdir = opendir(dir);
    struct dirent *pd = NULL;
    struct stat sbuf_dst;
    lstat(dir, &sbuf_dst);
    if(pdir == NULL)
    {
        fprintf(stderr, "open dir error\n");
        exit(1);
    }
    else
    {
        while((pd = readdir(pdir)) != NULL)
        {
            if(pd->d_name[0] == '.') continue;
            int total = 0;
            int len = strlen(dir)+2+strlen(pd->d_name);
            char* src_name = (char*)malloc(len);
            memset(src_name, 0x00, len);
            sprintf(src_name, "%s/%s", dir, pd->d_name);
            print_information(src_name);
        }
        closedir(pdir);
    }
}

void file_type(mode_t md)//显示文件类型函数
{

    char mode[11] = "?---------";
    //文件属性
    if((md & S_IFMT) == S_IFREG) mode[0] = '-';
    if((md & S_IFMT) == S_IFDIR) mode[0] = 'd';
    if((md & S_IFMT) == S_IFIFO) mode[0] = 'p';
    if((md & S_IFMT) == S_IFSOCK) mode[0] = 's';
    if((md & S_IFMT) == S_IFBLK) mode[0] = 'b';
    if((md & S_IFMT) == S_IFCHR) mode[0] = 'c';
    if((md & S_IFMT) == S_IFLNK) mode[0] = 'l';
    //文件权限
    if((md & S_IRUSR)) mode[1] = 'r';
    if((md & S_IWUSR)) mode[2] = 'w';
    if((md & S_IXUSR)) mode[3] = 'x';
    if((md & S_IRGRP)) mode[4] = 'r';
    if((md & S_IWGRP)) mode[5] = 'w';
    if((md & S_IXGRP)) mode[6] = 'x';
    if((md & S_IROTH)) mode[7] = 'r';
    if((md & S_IWOTH)) mode[8] = 'w';
    if((md & S_IXOTH)) mode[9] = 'x';

    printf("%s. ", mode);
}

void print_information(char* file)
{
    struct stat sbuf_reg;
    lstat(file, &sbuf_reg);

    //打印文件属性 权限信息
    file_type(sbuf_reg.st_mode);

    //硬链接个数
    printf("%d ",sbuf_reg.st_nlink);

    //文件拥有者；所在组
    uid_t my_uid;
    gid_t my_gid;
    struct passwd *myname;
    struct group *mygrp;
    myname = getpwuid(getuid());
    mygrp = getgrgid(getgid());
    printf("%2s ", myname->pw_name);
    printf("%2s ", mygrp->gr_name);

    //文件大小
    printf("%d ", sbuf_reg.st_size);

    //文件更新时间
    struct tm *block;
    block = localtime(&sbuf_reg.st_mtime);
    printf("%4d-%02d-%02d %02d:%02d ", 
        block->tm_year+1900, block->tm_mon+1, block->tm_mday,
        block->tm_hour, block->tm_min);

    //文件名
    printf("%s ",file);
    char buf[256] = {};
    if(S_ISLNK(sbuf_reg.st_mode))
    {
        printf("-> ");
        readlink(file, buf, 256);
        printf("%s", &buf);
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    lstat(argv[1], &sbuf);
    if(argc != 2)
    {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        exit(1);
    }

    if(S_ISDIR(sbuf.st_mode))
    {
        printf("总用量 %d\n", sbuf.st_blocks);
        open_dir(argv[1]);
    }

    else 
    {
        printf("总用量 %d\n", sbuf.st_blocks);
        print_information(argv[1]);
    }
}



