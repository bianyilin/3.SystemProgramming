/*************************************************************************
	> File Name: daemon.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月08日 星期四 16时49分00秒
 ************************************************************************/

#include<stdio.h>
#include"common.c"
#define NOFILE 1

void runAsDaemon(){
    int pid = -1;
    pid = fork();
    if(pid < 0){
        exit(1);
    } else if(pid > 0){
        exit(0);
    }else {
        setsid();
        pid = fork();
        if(pid < 0){
            exit(1);
        } else if(pid > 0){
            exit(0);
        }else{
            for(int fd = 0;fd < NOFILE;++fd){
                close(fd);
            }
            int fd = open("/dev/null",O_RDONLY);
            dup2(fd,0);
            fd = open("tmp/daemon_out.txt",O_RDWR | O_CREAT,0666);
            dup2(fd,1);
            fd = open("tmp/daemon_err.txt",O_RDWR | O_CREAT,0666);
            dup2(fd,2);
            chdir("/");
            umask(0);
        }
    }
}

int main() {
    runAsDaemon();
    printf("%s","Hello i am daemon std out\n");
    fprintf(stderr,"Hello i am daemon error out\n");
    return 0;
}
