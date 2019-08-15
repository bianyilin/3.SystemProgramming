/*************************************************************************
	> File Name: file_s2.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月03日 星期六 19时10分23秒
 ************************************************************************/

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
 
#define portnum 13795
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024
 
void *net_thread(void * fd);
 
int main()
{
	//初始化套接字
	int server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==server_fd)
	{
		perror("socket");
		exit(1);
	}
	//绑定端口和ip;
	struct sockaddr_in server_addr;   //struct sockaddr_in为结构体类型 ，server_addr为定义的结构体   
	server_addr.sin_family=AF_INET;   //Internet地址族=AF_INET(IPv4协议) 
	server_addr.sin_port=htons(portnum);  //将主机字节序转化为网络字节序 ,portnum是端口号
	(server_addr.sin_addr).s_addr=htonl(INADDR_ANY);//IP地址
	if(-1==bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)))  //套接字与端口绑定
	{
		perror("bind");
		exit(6);
	}
	//开启监听
	if(-1==listen(server_fd,5)) //5是最大连接数，指服务器最多连接5个用户
	{
		perror("listen");
		exit(7);
	}
	
	while(1)
	{
		struct sockaddr_in client_addr;
		int size=sizeof(client_addr);
		int new_fd=accept(server_fd,(struct sockaddr *)&client_addr,&size);  //server_fd服务器的socket描述字,&client_addr指向struct sockaddr *的指针,&size指向协议地址长度指针
 
		if(-1==new_fd)
		{
			perror("accept");
			continue;       //进行下一次循环
		}
		printf("accept client ip：%s:%d\n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port);
		//inet_ntoa将一个十进制网络字节序转换为点分十进制IP格式的字符串。
		
		printf("new_fd=%d\n",new_fd);
		
		// 打开文件，存入客户端的文件描述符
	    FILE *file_fp = fopen("01.file_fp", "w+"); 
        if(NULL == file_fp) 
        { 
             printf(" open 01.file_fp failure\n" ); 
             exit(1); 
        } 
	    else 
	    {
		     int a=new_fd;
		     fprintf(file_fp,"%d\n",new_fd);
		     fclose(file_fp);
	    }
			
		int pthread_id;
		int ret = pthread_create((pthread_t *)&pthread_id,NULL,net_thread,(void *)&new_fd);
		if(-1==ret)
		{
			perror("pthread_create");
			close(new_fd);
			continue;
		}
		
	}
	close(server_fd);
	return 0;
 
}
 
 
 
void *net_thread(void * fd)
{
	pthread_detach(pthread_self()); //线程分离
	int new_fd=*((int *)fd);
	int file2_fp;
	
	while(1)
	{
		// recv函数接收数据到缓冲区buffer中 
        char buffer[BUFFER_SIZE];
        memset( buffer,0, sizeof(buffer) );		
        if(read(new_fd, buffer, sizeof(buffer)) < 0) 
        { 
           perror("Server Recieve Data Failed:"); 
           break; 
        } 
		
        // 然后从buffer(缓冲区)拷贝到file_name中 
        char file_name[FILE_SIZE]; 
		memset( file_name,0, sizeof(file_name) );	
        strncpy(file_name, buffer, strlen(buffer)>FILE_SIZE?FILE_SIZE:strlen(buffer)); 
		memset( buffer,0, sizeof(buffer) );
        printf("%s\n", file_name); 
		
		if( strcmp(file_name,"null")==0 )
	    {
		   break;
		   close(new_fd);
	    }
		
		  // 打开文件并读取文件数据 
         file2_fp = open(file_name,O_RDONLY,0777); 
         if(file2_fp<0) 
         { 
            printf("File:%s Not Found\n", file_name); 
         } 
         else 
         { 
            int length = 0; 
			memset( buffer,0, sizeof(buffer) );
            // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
            while( (length = read(file2_fp, buffer, sizeof(buffer))) > 0  )    
            {   
                if( write(new_fd, buffer, length) < 0) 
                { 
                    printf("Send File:%s Failed.\n", file_name); 
                    break; 
                } 
                memset( buffer,0, sizeof(buffer) );
            } 
              // 关闭文件 
              close(file2_fp); 
              printf("File:%s Transfer Successful!\n", file_name); 
         }   
	}
	close(new_fd);
}

