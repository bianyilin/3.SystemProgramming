/*************************************************************************
	> File Name: file_c3.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月03日 星期六 19时48分30秒
 ************************************************************************/

#include<stdio.h>
#include "common.h"

#define PORT 13791
#define SIZE 1460

typedef struct File{
    char f_name[20];
    char file[SIZE];
} struct_file;

int send_file(int sock_fd) {
    struct_file file;
    strcpy(file.f_name, "send.txt""send1.txt");
   // strcpy(file.f_name, "send1.txt");
    memset(&file.file, 0, sizeof(file.file));
    FILE *fp;
    if ((fp = fopen(file.f_name, "r")) == NULL) {
        perror("fopen() error");
    }
    size_t size = sizeof(int) / 4;

    while(fread(file.file, size, 1024, fp) != 0) {
        if((send(sock_fd, &file, SIZE, 0)) != 0) {
            printf("success send %s!", file.f_name);
            memset(&file.file, 0, sizeof(file.file));
        } else if (file,size == 0){
            printf("此文件为空");
        }
        
    }
    printf("\n");
}

int main() {
    char address[20] = "192.168.2.89";
    int sock_fd;
    int port = PORT;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(address);
    if ((connect(sock_fd, (struct sockaddr *)&server, sizeof(server))) < 0) {
        perror("connect() error");
    }
    send_file(sock_fd);
    close(sock_fd);
    return 0;
}
