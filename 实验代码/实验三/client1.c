//
// Created by 纸蓝 on 2022/1/4.
//


#include <sys/socket.h>

#include <netinet/in.h>

#include <netinet/ip.h>

#include <stdlib.h>

#include <arpa/inet.h>

#include <stdlib.h>

#include <unistd.h>

#include <stdio.h>

main(int ac, char *av[]){

    int tcp_socket;

    struct sockaddr_in addr;

    int fd;

    char buf[512];

    int n;

    tcp_socket  =  socket(AF_INET,  SOCK_STREAM, 0);

    addr.sin_family = AF_INET;

    addr.sin_port = htons(atoi(av[2]));

    addr.sin_addr.s_addr = inet_addr(av[1]);

    if(connect(tcp_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in))==-1){

        perror("cannot connect");

        exit(1);

    }

    write(tcp_socket, "from client", 11);

    n=read(tcp_socket, buf, sizeof(buf));

    printf("%.*s\n", n, buf);

    close(tcp_socket);

}