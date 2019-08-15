/*************************************************************************
	> File Name: common.h
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月01日 星期四 09时48分34秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pwd.h>
#include <pthread.h>
#include <wait.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <sys/mman.h>

#ifndef _COMMON_H
#define _COMMON_H
int socket_create(int port) {
    int sockfd;
    struct sockaddr_in addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;

    }

    if (listen(sockfd, 20) < 0) {
        return -1;
    }
    return sockfd;
}


int socket_connect(char *host, int port) {
//int socket_connect(int port,char *ip){
    int sockfd;
    struct sockaddr_in server;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }
    return sockfd;
}

int get_conf_val(char *file, char *key, char *value){
    FILE *fp = NULL;
    char *line = NULL, *substr = NULL;
    size_t n = 0, len = 0;
    ssize_t read;

    if (key == NULL || value == NULL) {
        printf("error\n");
        return -1;
    }
    fp = fopen(file, "r");
    if (fp == NULL){
        printf("Open config file error!\n");
        return -1;
    }

    while((read = getline(&line, &n, fp)) != -1){
        substr = strstr(line, key);
        if (substr == NULL){
            continue;
        } else {
            len = strlen(key);
            if (line[len] == '='){
                strncpy(value, line + len + 1, (int)read - len - 2);
                break;
            } else {
                continue;
            }
        }
    
    }
    if(substr == NULL) {
        printf("%s Not found in %s!\n", key, file);
        fclose(fp);
        free(line);
        return -1;
    }

    fclose(fp);
    free(line);
    return 0;
}

char *my_inet_ntoa(struct in_addr in) {
    static char ip[20] = {0};
    char *p;
    p = (char *)&in.s_addr;
    sprintf(ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return ip;
}

int ctoi(char *str) {
    int len = strlen(str);
    int num[10];
    int port = 0;
    for (int i = 0; i < len; i++) {
        num[i] = str[i] - '0';
        port += num[i] * pow(10, len - 1 - i);
    }
    return port;
}
#endif
/*
 * 100 CUP
 * 101 MEMLOG
 * 102 DISK
 * 103 PRO 
 * 104 SYSINFO
 * 105 USERINOF
 */
