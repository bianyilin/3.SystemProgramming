/*************************************************************************
	> File Name: commonf.h
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月03日 星期六 15时31分02秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_LINE 1024
#define PORT 8000
#define ADDR_LEN 17
#define NAME_LEN 256

extern int init(struct sockaddr_in *sin, int *lfd, int sock_opt);
extern int do_put(int cfd, char *file);
extern int do_get(int cfd, char *file);

