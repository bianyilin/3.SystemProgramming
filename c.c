/*************************************************************************
	> File Name: c.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月11日 星期日 17时57分03秒
 ************************************************************************/

#include<stdio.h>
#include "common.h"
#define BUFFSIZE 64
#define PORT 8000
#define MASTPORT 8731

#define LOGFILE "client.log"

struct sm_msg{
    char buff[1024];
    pthread_mutex_t sm_mutex;
    pthread_cond_t sm_ready;
};

int *k;
void ko(){
    close(*k);
    exit(0);
}

void heart(char *addr, int port) {
    int tmpfd;
    int time = 0, ret = 0;
    while ((tmpfd = socket_connect(addr, MASTPORT)) < 0) {
        for (int i = 0; i <= time; i += 1) {
            sleep(1);
            printf("\033[31;1m*\033[0m");
            fflush(stdout);
        }
        printf("\n");
        if (ret != 10) time += 10;
        ret++;
    }
    printf("\033[31;1m♥\033[0m\n");
    close(tmpfd);
}

void sys_testing() {
    char *buff = NULL;
    FILE *fpr[10] = {0};
    FILE *fpw[10] = {0};
    char file[10][128] = {"user.sh", "disk.sh"};
    char temp[BUFFSIZE] = {0};
    for (int i = 0; i < 2; i++) {
        sscanf(file[i], "%s.sh", temp);
        printf("%s\n", temp);
        strcat(temp, ".log");
        printf("%s\n", temp);
        if ((fpw[i] = fopen(temp, "a+")) == NULL) {
            perror("fopen");
            
        }
        memset(temp, 0, sizeof(temp));
    }
    for (int i = 0; i < 2; i++) {
        sprintf(temp, "bash ./%s", file[i]);
        printf("%s\n", temp);
        fpr[i] = popen(temp, "r");
        memset(temp, 0, sizeof(temp));
    }
    while (1) {
        for (int i = 0; i < 2; i++) {
            size_t len = 0;
            int size;
            while ((size = getline(&buff, &len, fpr[i])) > 0) {
                printf("%s\n", buff);
                if(fwrite(buff, 1, size, fpw[i]) < 0) {
                    perror("fwrite");
                }
            }
        }
        sleep(100);
    }
}

void active_check(struct sm_msg *msg, int listen) {
    fd_set read;
    FD_ZERO (&read);
    FD_SET(listen, &read);
    struct timeval tv;
    int ret = 0;
    while (1) {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int r = select(listen + 1, &read, NULL, NULL, &tv);
        ret++;
        if (r > 0) {
            ret = 0;
            FD_ZERO(&read);
            FD_SET(listen, &read);
            continue;
        } else {
            printf(" %d\n", ret);
            if (ret >= 5) {
                ret = 0;
                pthread_cond_signal(&msg->sm_ready);
                pthread_cond_wait(&msg->sm_ready, &msg->sm_mutex);
            
            }
        }
    }
}

void sendfile(int sockfd, char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("403\n");
        return ;
    }
    int num_read;
    char *data = NULL;
    size_t len = 0;
    while ((num_read = getline(&data, &len, fp)) != -1) {
        if (send(sockfd, data, strlen(data), 0) < 0){
            perror("send");
            fclose(fp);
            return ;
        }
    }
    fclose(fp);
}

void send_file(char *addr) {
    int sockfd, port, sock_file;
    char *p;
    FILE *fp;
    char path[20];
    strcpy(path, "test.log");
    port = 8731;
    char *ip = "192.168.2.89";
    while (1) {
        if ((sockfd = socket_connect(ip, port)) < 0) {
            exit(1);
        }
        if ((send(sockfd, path, strlen(path), 0)) < 0) {
            perror("send");
        }
        int ack;
        if (recv(sockfd, &ack, sizeof(ack), 0) <= 0 ) {
            perror("recv ack");
        }
        if (ack == 1) {
            sendfile(sockfd, path);
            close(sock_file);
        }
    }
    close(sockfd);
}

int main() {
    int sockfd, listen, shmid;
    char *share_memory;
    signal(SIGINT, ko);
    char serveraddr[20] = "192.168.2.89";
    pid_t p;
    if ((listen = socket_create(PORT)) < 0) {
        perror("socket_create");
        exit(1);
    }
    pthread_mutexattr_t m_attr;
    pthread_condattr_t c_attr;

    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);

    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);


    if ((shmid = shmget(IPC_PRIVATE, sizeof(struct sm_msg), 0666|IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    if ((share_memory = shmat(shmid, 0, 0)) == NULL) {
        perror("shmat");
        exit(1);
    }

    struct sm_msg *msg = (struct sm_msg *)share_memory;

    pthread_mutex_init(&(msg->sm_mutex), &m_attr);
    pthread_cond_init(&msg->sm_ready, &c_attr);
    p = fork();
    if (p == 0) {
        active_check(msg, listen);
        exit(1);
    }
    p = fork();
    if (p == 0) {
        sys_testing();
        exit(1);
    }
    p = fork();
    if (p == 0) {
        while (1) {
            pthread_cond_wait(&msg->sm_ready, &msg->sm_mutex);
            heart(serveraddr, PORT);
            pthread_cond_signal(&msg->sm_ready);
        }
        exit(1);
    }

	int on = 1;  
    if((setsockopt(listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))<0) {  
        perror("setsockopt failed");  
        exit(1);  
    }  
    while (1) {
        if ((sockfd = accept(listen, NULL, NULL)) < 0) {
            perror("accept");
            usleep(1000);
            continue;
        }
        char buff[BUFFSIZE] = {0};
        if (recv(sockfd, buff, BUFFSIZE, 0) > 0) {
            printf("\033[31;1m%s\n\033[0m", buff);
            send(sockfd, buff, strlen(buff), 0);
            close(sockfd);
        } else {
            close(sockfd);
        }
    }
    return 0;
}
