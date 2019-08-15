/*************************************************************************
	> File Name: file_c1.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月03日 星期六 17时49分54秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include"common.h"
int main() {
    char buf[1024],file[30];
    char *strs="conveying,waiting...";
    int target;
    register int k,s;
    struct sockaddr_in sin;
    struct hostent *hp;
    system("clear");
    printf(" ");

    hp = gethostbyname("server");
    if(hp == NULL){
        perror("fault to return server address");
        exit(1);
    }
    s = socket(AF_INET,SOCK_STREAM,0);
    if(s < 0){
        perror("fault to obbtain socket");
        exit(2);
    }
    sin.sin_family=AF_INET;
    sin.sin_port=htons(1500);
    bcopy(hp->h_addr,&sin.sin_addr,hp->h_length);
    printf("connecting the server...");
    if(sock_connect(s,&sin,sizeof(sin),0)<0){
        perror("cannot connect server!");
        exit(3);
    }
    while((k=read(s,file,sizeof(file))))
    if((target=open(file,O_WRONLY|O_CREAT|O_TRUNC,0644))<0){
        perror("cannot open file");
        exit(4);
    }
    strcat(strs,file);
    strcat(strs,"waiting...");
    write(1,strs,strlen(strs));
    while((k=read(s,buf,sizeof(buf)))>0)
    write(target,buf,k);
    printf("ok");
    close(s);
    close(target);
}

