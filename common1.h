/*************************************************************************
	> File Name: common1.h
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月11日 星期日 18时05分47秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#ifdef D

#define DBG(a,b) printf(a,b)

#else

#define DBG(a,b)
#endif

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdarg.h>
#include <errno.h>

#define __MAX_N 1024

#define P(sem) sem_wait(sem)

#define V(sem) sem_post(sem)

int socket_create(int);

int socket_connect(char *, int );

int socket_connect_nonblock(char *, int, struct timeval *);

int socket_accept(int sock_listen);

void *stop_out(int);

int socket_accept(int );

void my_sleep(int);

int get_conf_value(char *file, char *key, char *val); 

void strreplace(char *s, char *a, char *b);

void _write_log(const char *errorfile, const int line, const char *Logfile,  const char *fmt, ...);

#define write_log(Logfile, fmt,...) _write_log(__FILE__, __LINE__, Logfile, ##__VA_ARGS__)

#endif
