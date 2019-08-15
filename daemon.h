/*************************************************************************
	> File Name: daemon.h
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月08日 星期四 18时36分29秒
 ************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#ifndef _DAEMON_H
#define _DAEMON_H
void write_log(char *); 
void set_daemon(void);
void signal_handler(int);
void listen_fifo();
#endif
