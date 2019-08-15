/*************************************************************************
	> File Name: client_s.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月01日 星期四 09时52分33秒
 ************************************************************************/

#include"commom1.h"
#include<stdbool.h>
#define PORT 5679
#define BUFFSIZE 1024

bool client_heart(char *ip,int port){
    bool ret = true;
    char hostname[20] = {0};
    struct passwd *pwd;
    int fd;
    if ((fd = socket_connect(port,ip)) < 0){
        ret = false;
    }
    gethostname(hostname,sizeof(hostname));
    pwd = getpwuid(getuid());
    char buf[BUFFSIZE] = {0};
    sprintf(buf,"Login -> %s on %s!",pwd->pw_name,hostname);
    send(fd,buf,strlen(buf),0);
    memset(buf,0,sizeof(buf));
    while(1) {
        if (recv(fd,buf,sizeof(buf),0) <= 0) {
            close(fd);
            break;
        } else {
            printf("%s\n",buf);
        }
    }
    printf("✌\n");
    return ret;
}

int main(){
    int sockfd,listen,pid,heart_listen,heart_port = 8765;
    char *master_ip = "192.168.2.89";
    int master_port = 5679;


    if ((listen = socket_create(PORT)) < 0) {
        perror("socket_create");
        exit(1);
    }

    if ((pid = fork()) < 0){
        perror("fork erroe");
        exit(1);
    }

    if (pid == 0) {
        
        while(1) {
            if ((sockfd = accept(listen,NULL,NULL)) < 0){
                perror("accept");
                continue;
            }
            char buff[BUFFSIZE] = {0};
            char buff1[BUFFSIZE] = {0};
            if (recv(sockfd,buff,BUFFSIZE,0) > 0){
                printf("%s\n",buff);
                sprintf(buff1,"client:%s",buff);
                send(sockfd,buff1,strlen(buff1),0);
                close(sockfd);
            } else {
                close(sockfd);
            }
        }
    } else {
        int pid1;
        close(listen);

        if ((pid1 = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid1 == 0) {
            client_heart(master_ip,master_port);
        } else {
            if ((heart_listen = socket_create(heart_port)) < 0){
                perror("heart_listen");
                exit(1);
            }
            while(1) {
                int fd;
                if ((fd = accept(heart_listen,NULL,NULL)) < 0) {
                    perror("accept");
                    close(fd);
                    continue;
                }
                printf("\033[35m \033[0m");
                fflush(stdout);
                char temp[BUFFSIZE];
                if (recv(fd,temp,sizeof(temp),0) == 0){
                    close(fd);
                }
            }
        }
    }
return 0;
}

