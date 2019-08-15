/*************************************************************************
	> File Name: client.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月01日 星期四 09时49分58秒
 ************************************************************************/

#include "common1.h"
#define M_PORT 5679

int socket_connect(char *host, int port);
int socket_create(int port);

char name[20] = {"bianyilin"}; 
int sockfd;


void do_con(char  *host,  int  port)  {
    int  retime  =  2;
    while  (1)  {
        int  sockfd  =  socket_connect(host,port);
        if  (sockfd  <  0)  {
            close(sockfd);
            sleep(retime);
            if  (retime  <  100)
            retime  +=  10;
            printf("try  to  connect\n");
            continue;
            }
            close(sockfd);
            break;
        }
}


void signalFun() {
    close(sockfd);
    printf("\033[;38;5m程序退出\033[0m\n");
    exit(0);
}
int do_put(int cfd,char *file){
    struct stat statbuf;
    int n,fd;
    int res = -1;
    char buf[1024];

    if ((fd = open(file,O_WRONLY | O_CREAT|O_TRUNC,0644)) == -1){
        printf("cannot open the file\n");
        write(cfd,"Error open master file\n",strlen("Error open master file\n"));
        close(fd);
        return -1;
    }
    if ((fstat(fd,&statbuf)) == -1){
        printf("connot get the stat of file\n");
        write(cfd,"Error stat master file\n",strlen("Error stat server file\n"));
        close(fd);
        return -1;
    }
    if (!S_ISREG(statbuf,st_mode)){
        
    }
}

int main() {
    int listen_fd;
    char ip[20] = {0};
    char ports[10] = {0};
    int port;
    get_conf_val("ma.conf", "master_ip", ip);
    get_conf_val("ma.conf", "master_port", ports);
    port = ctoi(ports);
    signal(2, signalFun);
    pid_t pid = fork();
    if (pid != 0) {
        int new_fd;
        struct sockaddr_in peer_addr;
        socklen_t len = sizeof(struct sockaddr);
        if ((listen_fd = socket_create(M_PORT)) < 0) {
            perror("socket_create");
            exit(1);
        }
        while (1) {
            new_fd = accept(listen_fd, (struct sockaddr *)&peer_addr, &len);
        }
    }
    if (pid == 0) {
        char heart;
        int i = 1;
        while (i <= 10) {
            if ((sockfd = socket_connect(ip, port)) < 0) {
                printf("\033[;36;5m第%d次连接:\033[0m\n", i);
                for (int j = 0; j < i; j++) {
                    printf("\033[;31;5m💖 \033[0m");
                }
                printf("\n");
                sleep(i);
                i++;
            } else {
                printf("\033[;37;5m连接成功!\033[0m\n");
                break;
            }
        }
        send(sockfd, &name, sizeof(name), 0);
        while (1) {
            if (recv(sockfd, &heart, 1, 0) < 0) {
                perror("recv");
                exit(1);
            }
        }
        
    }
    return 0;
}

