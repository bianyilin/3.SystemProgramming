/*************************************************************************
	> File Name: common1.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月11日 星期日 18时05分36秒
 ************************************************************************/

#include<stdio.h>
#include "common1.h"

int socket_create(int port) {
    int sockfd;
    struct sockaddr_in addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int on = 1;
    if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))<0) {  
        perror("setsockopt failed");  
        exit(1);  
    }  

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;

    }

    if (listen(sockfd, 20) < 0) {
        return -1;
    }
    return sockfd;
}

void *stop_out(int stat) {
    exit(0);
}

int socket_connect_nonblock(char *host, int port, struct timeval *tv) {
    int sockfd, ret;
    struct sockaddr_in server;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);


    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);
    
    fd_set write;
    FD_ZERO(&write);
    FD_SET(sockfd, &write);

    int error = -1;
    int len;
    len = sizeof(error);
    ret = sockfd;

    if ((connect(sockfd, (struct sockaddr *)&server, sizeof(server))) < 0) {
        if ((select(sockfd + 1, NULL, &write, NULL, tv)) > 0) {
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0) {
                ret -1;
            }
            if (error == 0) {
                ret = 0;
            } else {
                ret = -1;
            }
        }
    } else {
        printf("XXXXXXXXXXXXXXXXXXXXXXXX\n");
    }

    return ret;
}

int socket_connect(char *host, int port) {
    int sockfd;
    struct sockaddr_in server;
        
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);
    if ((connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)) {
        return -1;
    } 
    return sockfd;
}
int socket_accept(int server_listen) {
    int sockfd = accept(server_listen, NULL, NULL);
    return sockfd;
}

void tmp() {
    return;
}
void my_sleep(int time) {
    signal(SIGALRM, tmp);
    alarm(time);
    pause();
}

int get_conf_value(char *file, char *key, char *val) {
    FILE *fp = NULL;
    char *line = NULL, *substr = NULL;
    size_t n = 0, len = 0;
    ssize_t read;

    if (key == NULL) {
        printf("error\n");
        return -1;
    }
    fp = fopen(file, "r");

    if (fp == NULL) {
        printf("Open config file error!\n");
        return -1;
    }

    while ((read = getline(&line, &n, fp))) {
        substr = strstr(line, key);
        if (substr == NULL) {
            continue;
        } else {
            len = strlen(key);
            if (line[len] == '=') {
                strncpy(val, line + len + 1, (int)read - len - 2);
                break;
            } else {
                continue;
            }
        }
    }
    if (substr == NULL) {
        printf("%s Not found in %s!\n", key, file);
        fclose(fp);
        free(line);
        return -1;
    }
    fclose(fp);
    free(line);
    return 0;
}

void strreplace(char *s, char *a, char *b) {
    typedef long long lint;
    char * pos = NULL;
    char ans[__MAX_N + 5] = {0};
    pos = strstr(s, a);
    if (!pos) return ;
    strncpy(ans, s, pos - s);
    strcat(ans, b);
    strcat(ans, pos + strlen(a));
    strcpy(s, ans);
    strreplace(s, a, b);
    return ;
} 

void _write_log(const char *errorfile, const int line, const char *Logfile,  const char *fmt, ...) {
    FILE *fp = fopen(Logfile, "a+");
    flock(fp->_fileno, LOCK_EX);
    int done;
    va_list arg;
    va_start(arg, fmt);
    time_t time_log = time(NULL);
    struct tm *tm_log =localtime(&time_log);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [%s: %d] ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec,errorfile, line);
    done = vfprintf(fp, fmt, arg);
    va_end(arg);
    fflush(fp);
    fclose(fp);
    return ;
}


