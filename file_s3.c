/*************************************************************************
	> File Name: file_s3.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月03日 星期六 19时48分51秒
 ************************************************************************/

#include<stdio.h>
#include "common.h"

#define PORT 13791
#define SIZE 1460

typedef struct File{
    char name[20];
    char file[SIZE];
} struct_file;

void recv_file(int new_sock);

int accept_clenct(int sock_fd, int port) {
    int new_sock;
    struct sockaddr_in my_addr;
    struct sockaddr_in peer_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("bind() error");
    }
    listen(sock_fd, 20);
    socklen_t len = sizeof(struct sockaddr);
    while((new_sock = accept(sock_fd, (struct sockaddr *) &peer_addr, &len)) > 0) {
        int pid = fork();
        if (!pid) recv_file(new_sock);
    }
    return 0;
}
void recv_file(int new_sock) {
    struct_file recv_file;
    strcpy(recv_file.name, "recv.txt");
    memset(&recv_file.file, 0, sizeof(recv_file.file));
    FILE *fp;
    if ((fp = fopen(recv_file.name, "a+")) == NULL) {
        perror("fopen() error");
    }
    size_t size = sizeof(int) / 4;
    while((recv(new_sock, &recv_file, SIZE, 0)) > 0) {
        fwrite(recv_file.file,size, sizeof(recv_file.file), fp);
        memset(&recv_file.file, 0, sizeof(recv_file.file));
    }  

    if (recv_file.file == NULL) {
        printf("接收文件为空");
    } else {
        printf("已接收%s!\n", recv_file.name);

    }
    fclose(fp);
    close(new_sock);
    exit(0);
}

int main() {
    int port = PORT;
    int sock_fd;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
    }
    accept_clenct(sock_fd, port);
    close(sock_fd);
    return 0;
}
