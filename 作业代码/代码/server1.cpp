#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

// 设置sockfd
int InitSocket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        return -1;
    }

    struct sockaddr_in ser;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(80);  // root用户下运行
    ser.sin_addr.s_addr = inet_addr("10.211.55.16");

    int res = bind(sockfd, (struct sockaddr*)&ser, sizeof(ser));
    if (res == -1)
    {
        return -1;
    }

    res = listen(sockfd, 5);
    if (res == -1)
    {
        return -1;
    }

    return sockfd;
}

// 接收并解析客户端发送的请求报头
int RecvData(int c, char *file_name)
{
    char buff[1024] = { 0 };
    int n = recv(c, buff, 1023, 0);
    if (n <= 0)
    {
        return -1;
    }

    //printf("%s\n", buff);

    // 剪切请求报头的请求页面
    char *p = strtok(buff, " ");  // p -> GET
    p = strtok(NULL, " "); // p -> "/index.html"

    char path[128] = "/home/zhilan/Desktop/www";

    // 给file_name中填充的是系统上肯定存在的文件
    strcat(path, p);  // path:"/var/www/html/index.html"

    // 判断文件是否存在
    int test = access(path, F_OK);
    if (test == -1)
    {
        strcpy(file_name, "/home/zhilan/Desktop/www/404.html");
    }
    else
    {
        strcpy(file_name, path);
    }

    return 0;
}

// 向客户端发送应答报头
void SendHead(int c, int flag, int size)
{
    char buff[1024] = { 0 };

    //http协议版本
    strcat(buff, "HTTP/1.0 ");

    // 状态码
    if (flag)
    {
        strcat(buff, "404 Not Found\r\n");
    }
    else
    {
        strcat(buff, "200 OK\r\n");
    }

    // 服务器名称
    strcat(buff, "Server: MYWEB/1.0\r\n");

    // 数据长度
    strcat(buff, "Content-Length: ");
    sprintf(buff + strlen(buff), "%d", size);
    strcat(buff, "\r\n");

    strcat(buff, "Content-Type: text/html; charset=utf-8\r\n");
    strcat(buff, "\r\n");

    send(c, buff, strlen(buff), 0);
}

// 向客户端发送应答页面数据
int SendData(int c, char *file_name)
{
    // 1.给浏览器发送头部结构
    // 2.给浏览器发送页面数据：file_name文件中的内容

    int flag = 0;
    if (strstr(file_name, "404.html") != NULL) // 404 not found
    {
        flag = 1;
    }

    // 获取需要发送的文件的信息
    struct stat st;
    stat(file_name, &st);
    SendHead(c, flag, st.st_size); // 文件描述符 状态信息 数据长度

    // 发送文件
    int fd = open(file_name, O_RDONLY);
    while (1)
    {
        char buff[128] = { 0 };
        int n = read(fd, buff, 127);
        if (n <= 0)
        {
            break;
        }

        n = send(c, buff, n, 0);
        if (n <= 0)
        {
            return -1;
        }
    }

    close(fd);
    return 0;
}


int main()
{
    int sockfd = InitSocket();
    assert(sockfd != -1);
    char buf[1024];
    char cmd[512];
    char path[512];
    int rv_fork;

    while (1)
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        int c = accept(sockfd, (struct sockaddr*)&cli, &len);
        //实现多线程
//        int n=read(c, buf, sizeof(buf));
//
//        sscanf(buf, "%s%s", cmd, path);
//
//        if(strcmp(cmd, "GET")==0){
//
//            rv_fork=fork();
//
//            if(0==rv_fork){
//
//                //RecvData(c, file_name);
//
//                close(c);
//
//                exit(0);
//
//            }else{
//
//                close(c);
//
//            }
//
//        }
        if (c < 0)
        {
            printf("accept error");
            close(c);
            continue;
        }

        char file_name[128] = { 0 };
        int num = RecvData(c, file_name);
        if (num == -1)
        {
            close(c);
            continue;
        }

        num = SendData(c, file_name);
        if (num == -1)
        {
            close(c);
            continue;
        }

        close(c);  // 短链接方式
    }


    close(sockfd);
    exit(0);
}



