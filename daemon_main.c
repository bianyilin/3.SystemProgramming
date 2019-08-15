/*************************************************************************
	> File Name: daemon_main.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月08日 星期四 18时37分54秒
 ************************************************************************/

#include<stdio.h>
#include <signal.h>
#include <unistd.h>
#include "daemon.h"
 
int main() {
    set_daemon();
    signal(SIGINT, signal_handler);
    while(1) {
        sleep(1);
    }   
    return 0;
}

