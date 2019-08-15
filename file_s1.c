/*************************************************************************
	> File Name: file_s1.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月03日 星期六 17时52分11秒
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "common.h"
int main(){
    char c,buf[1024],file[30];
    int fromlen,source;
    register int k,s,ns;
    struct sockaddr_in sin;
    struct hostent *hp;
    system("clear");
    printf(" ");

    printf("input filename:");
    scanf("%s",file);
    if((source=open(file,O_RDONLY))<0){
        perror("open error");
        exit(1);
    }
    printf("conveying,waiting...");
    hp=gethostbyname("server");
    if(hp==NULL){
        exit(2);
    }
    s= socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        perror("fault to obtain socket");
        exit(3);
    }
    sin.sin_family=AF_INET;
    sin.sin_port=htons(1500);

    bcopy(hp->h_addr,&sin.sin_addr,hp->h_length);
    if(bind(s,&sin,sizeof(sin))<0){
        perror("connect tie to socket");
        close(s);
        exit(4);
    }
    if(listen(s,5)<0){
        perror("sever:listen");
        exit(5);
    }
    while(1){
        if((ns=accept(s,&sin,&fromlen))<0){
            perror("sever:accept");
            exit(6);
        }
        lseek(source,OL,0);
        write(ns,file,sizeof(file));
        while((k=read(source,buf,sizeof(buf)))>0)
        write(ns,buf,k);
        printf("ok");
        close(ns);
    }
    close(source);
    exit(0);
}

