/*************************************************************************
	> File Name: daemon.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月08日 星期四 18时34分38秒
 ************************************************************************/

#include "daemon.h"
 
const char *filename = "/home/solariens/Documents/C/err.log";
const char *fifo_path = "/home/solariens/Documents/C/fifo";
 
void write_log(char *error) {
    time_t date_time;
    time(&date_time);
    struct tm *date = localtime(&date_time);
 
    int year = date->tm_year;
    int month = date->tm_mon;
    int day = date->tm_mday;
    int hour = date->tm_hour;
    int min = date->tm_min;
    int sec = date->tm_sec;
 
    char str_date[1000];
    memset(str_date, 0, strlen(str_date));
    sprintf(str_date, "[date:%04d-%02d-%02d %02d:%02d:%02d] [err_msg:%s] \n", year, month, day, hour, min, sec, error);
    FILE *fp = fopen(filename, "a");
    fwrite(str_date, 1, strlen(str_date), fp);
    fclose(fp);
}
 
void set_daemon() {
    pid_t pid = fork();
    if (pid < 0) {
        write_log(strerror(errno));
        exit(0);
    } else if (pid > 0) {
        exit(0);
    } else {
        setsid();
        chdir("/");
        umask(0);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
}
 
void signal_handler(int signo) {
    switch(signo) {
        case SIGINT :
            listen_fifo();
            break;
    }
}
 
void listen_fifo() {
    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1) {
        write_log(strerror(errno));
        exit(0);
    }
    char buf[1000];
    memset(buf, 0, sizeof(buf));
    if((read(fd, buf, sizeof(buf))) > 0) {
        int len = strlen(buf);
        if (buf[len - 1] == '\n') {
            buf[len - 1] = 0;
        }
        write_log(buf);
    }
}


