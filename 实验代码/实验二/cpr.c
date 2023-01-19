#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void init_abs_path(char *bufsrc,char *bufdes,char *bufsrcfile,char *bufdesfile,const char *filename){
    memset(bufsrcfile,0,1024);
    memset(bufdesfile,0,1024);
    strcpy(bufsrcfile,bufsrc);
    strcat(bufsrcfile,filename);
    //printf("init  bufsrcfile %s\n",bufsrcfile);
    strcpy(bufdesfile,bufdes);
    strcat(bufdesfile,filename);
    //printf("init bufdesfile %s\n",bufdesfile);
}

void get_abs_path(char *bufsrc,char *bufdes, const char *dirsrc, const char *dirdes){
    char buf[1024];
    getcwd(buf,1024); // 获取当前路径
    chdir(dirsrc); // 进入source路径
    getcwd(bufsrc,1024); // 获取source的绝对路径
    strcat(bufsrc,"/");
    //printf("src:%s\n",bufsrc);

    chdir(buf);// 回到当前路径
    //printf("cur:%s\n",buf);
    DIR * des=opendir(dirdes);
    if(des==NULL){
        int rv=mkdir(dirdes,0777);
        if(-1==rv){
            perror("make directory ");
            exit(1);
        }
    }
    chdir(dirdes);// 进入des路径
    getcwd(bufdes,1024);// 获取des绝对路径
    strcat(bufdes,"/");
    //printf("des:%s\n",bufdes);

    chdir(buf);
}

void cp_content(char *src,char *des){
    int n;
    char buf[1024];
    int src_fd=open(src,O_RDONLY);
    if(src_fd < -1){
        perror("Src open failed");
        exit(1);
    }

    int des_fd=open(des,O_WRONLY|O_CREAT,0644);
    if(des_fd < -1){
        perror("Des open failed");
        exit(1);
    }

    while((n=read(src_fd,buf,sizeof(buf)))>0){
        if((write(des_fd,buf,n))==-1){
            perror("Write error");
        }
    }
}

void cp_dir(char * dirsrc,char * dirdes){
    // 存放目录绝对路径
    char bufsrc[1024]={};
    char bufdes[1024]={};
    // 存放文件绝对路径
    char bufsrcfile[1024]={};
    char bufdesfile[1024]={};

    DIR *dp;
    struct dirent *p;

    get_abs_path(bufsrc,bufdes,dirsrc,dirdes);

    //char buf[1024];
    //printf("%s\n",getcwd(buf,1024));
    int rv=chdir(dirsrc);
    if(-1==rv){
        perror("chdir");
        exit(1);
    }
    //memset(buf,0,1024);
    //printf("%s\n",getcwd(buf,1024));

    dp=opendir(dirsrc);
    if(dp==NULL){
        perror("Cannot open ");
        exit(1);
    }
    p=readdir(dp);
    while(p!=NULL){
        if(p->d_name[0]!='.'){
            //printf("%s---%s;\n",bufsrc,bufdes);
            init_abs_path(bufsrc,bufdes,bufsrcfile,bufdesfile,p->d_name);
            struct stat sb;
            if(stat(p->d_name,&sb)==-1){
                perror("stat");
                exit(0);
            }
            if(S_IFDIR==(sb.st_mode & S_IFMT)){
                //printf("cp %s to %s;\n",bufsrcfile,bufdesfile);
                cp_dir(bufsrcfile,bufdesfile);
            }else{
                cp_content(bufsrcfile,bufdesfile);
            }
            chdir(dirsrc);
        }
        p=readdir(dp);
    }
    closedir(dp);
}

int main(int ac, char* av[]){
    if(ac<3){
        printf("Usage CMD source destination\n");
        exit(1);
    }

    cp_dir(av[1],av[2]);
}

