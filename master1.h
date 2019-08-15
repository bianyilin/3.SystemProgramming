/*************************************************************************
	> File Name: master1.h
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月11日 星期日 18时08分32秒
 ************************************************************************/

#include "common1.h"
#define CTLPORT 8000
#define LOGFILE "master.log"
#define BUFFSIZE 1024

#ifdef PE
#define PERROR(a) PERROR(a)
#else 
#define PERROR(a)
#endif

typedef struct Node{
    struct sockaddr_in client_addr;
    int node_sock;
    struct Node *next;
}Node, *LinkedList;

struct HEART{
    LinkedList *head;
    int ins;
    int *sum;
    long timeout;
};


struct WORK{
    LinkedList head;
    int index;
};

typedef struct Error{
    char num[500][30];
} Error;


struct date{
    int sockfd;
    char file[BUFFSIZE];
    char ip[20];
};


int insert(LinkedList head, Node *node);
int find_min(int *sum, int ins);
void print_linkedlist(LinkedList head);
void *heartbeat_all(void *arg);
void *work(void *arg);
int check_connect(struct sockaddr_in, long timeout);
int do_event(struct sockaddr_in);
int socket_connect1(struct sockaddr_in);
int do_epoll(int server_listen, LinkedList *linkedlist, int *sum, int ins, int port);



int insert(LinkedList head, Node *node) {
    Node *p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = node;
    return 0;
}

int find_min(int *sum, int ins) {
    int sub = 0;
    for (int i = 0; i < ins; i++){
        if (*(sum + i) < *(sum + sub)) {
            sub = i;
        }
    }
    return sub;
}

void print_linkedlist(LinkedList head){
    Node *p = head;
    int cnt = 0;
    while( p->next != NULL ) {
        char tmp[BUFFSIZE];
        sprintf(tmp, "<%d> %s\n", cnt++, inet_ntoa(p->client_addr.sin_addr));
        write_log(LOGFILE, "%s\n", tmp);
        p = p->next;
    }
}


int check_connect(struct sockaddr_in addr, long timeout){
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        PERROR("socket");
        write_log(LOGFILE, "%s\n", strerror(errno));
        return 0;
    }
    int error = -1, len, ret = 0;
    struct timeval tv;
    unsigned long ul = 1;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;
    len = sizeof(int);
    ioctl(sockfd, FIONBIO, &ul);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(sockfd, &set);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (select(sockfd + 1, NULL, &set, NULL, &tv) > 0) {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            if (error == 0) {
                ret = 1;
            } else
            ret = 0;
        }
    }
    close(sockfd);
    return ret;
}



void  *heartbeat_all(void *arg) {
    struct HEART *heart = (struct HEART *)arg;

    while (1) {
        for (int i = 0; i < heart->ins; i++) {
            Node *p = heart->head[i];
            while (p->next != NULL) {
                if (!check_connect(p->next->client_addr, heart->timeout)) {
                    char tmp[BUFFSIZE];
                    sprintf(tmp, "%s:%d \033[31mdeleting\033[0m ... \n", inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
                    write_log(LOGFILE, "%s\n", tmp);
                    Node *q;
                    q = p->next;
                    p->next = p->next->next;
                    free(q);
                    heart->sum[i]--;
                } else {
                    char tmp[BUFFSIZE];
                    sprintf(tmp, "%s:%d \033[32monline\033[0m ... \n", inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
                    write_log(LOGFILE, "%s\n", tmp);
                    p = p->next;
                }
            }
        }
        sleep(2);
    }
    return NULL;
}



void *work(void *arg) {
    struct WORK *inarg = (struct WORK *)arg;
    char log[50] = {0};
    sprintf(log, "./%d.log", inarg->index);
    while (1) {
        FILE *file = fopen(log, "w");
        Node *p = inarg->head;

        while(p->next != NULL) {
            fprintf(file, "%s:%d\n", inet_ntoa(p->next->client_addr.sin_addr), ntohs(p->next->client_addr.sin_port));
            do_event(p->next->client_addr);
            p = p->next;
        }
        fclose(file);
        sleep(5);
    }

    return NULL;
}

int socket_connect1(struct sockaddr_in addr) {
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        PERROR("socket");
        write_log(LOGFILE, "%s\n", strerror(errno));
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        PERROR("connect");
        write_log(LOGFILE, "%s\n", strerror(errno));
        return -1;
    }

    return sockfd;
}




int do_event(struct sockaddr_in addr) {
    int sockfd;
    char buff[BUFFSIZE] = {0};
    addr.sin_port = htons(CTLPORT);
    if ((sockfd = socket_connect1(addr)) < 0) {
        PERROR("socket_connect1");
        write_log(LOGFILE, "%s\n", strerror(errno));
        return -1;
    }
    send(sockfd, "💖 ", strlen("💖"), 0);

    if (recv(sockfd, buff, BUFFSIZE, 0) <= 0) {
        close(sockfd);
        return 0;
    }
    close(sockfd);
    char tmp[BUFFSIZE];
    sprintf(tmp, "%s\n", buff);
    write_log(LOGFILE, "%s\n", tmp);
    return 0;
}

int do_epoll(int server_listen, LinkedList *linkedlist, int *sum, int ins, int port) {
    #define NUM_MAX 1024
    int sockfd, nfds, epfd;
    epfd = epoll_create(NUM_MAX);
    struct epoll_event ev, events[NUM_MAX];
    ev.data.fd = server_listen;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_listen, &ev);
    while (1) {
        for (int i = 0; i < ins; i++)  {
            nfds = epoll_wait(epfd, events, NUM_MAX, 1000);
            for (int i = 0; i < nfds; i++) {
                if (events[i].data.fd != server_listen) {
                    continue;
                }
                struct sockaddr_in claddr;
                memset(&claddr, 0, sizeof(claddr));
                socklen_t len = sizeof(struct sockaddr);
                if ((sockfd = accept(server_listen, (struct sockaddr *)&claddr, &len)) < 0) {
                    usleep(1000);
                    continue;
                } else {
                    char tmp[BUFFSIZE];
                    sprintf(tmp, "%s:%d \033[32monline\033[0m ... \n", inet_ntoa(claddr.sin_addr), ntohs(claddr.sin_port));
                    write_log(LOGFILE, "%s\n", tmp);
                    claddr.sin_port = htons(port);
                    Node *p = (Node *)malloc(sizeof(Node));
                    p->client_addr = claddr;

                    p->node_sock = sockfd;
                    p->next = NULL;
                    insert(linkedlist[i], p);
                }
            }
        }
    }
}

Error error;

void *_recv_file(void *tmp) {
    struct date *p = (struct date *)tmp;
    int server_fd = p->sockfd;
    char buff[BUFFSIZE + 5] = {0};
    char filename[BUFFSIZE];
    strcpy(buff, p->file);
    if (recv(server_fd, buff, sizeof(buff), 0) < 0) {
        perror("recv");
    }
    sprintf(filename, "master.%s", buff);
    int ack = 1;
    if ((send(server_fd, &ack, sizeof(ack), 0)) < 0) {
        perror("send");
    }
    FILE *fp = fopen(filename, "a+");
    
    while (1) {
        int size;
        memset(buff, 0, sizeof(buff));
        if ((size = recv(server_fd, buff, sizeof(buff), 0)) > 0) {
            printf("%s\n", buff);
            fwrite(buff, 1, size, fp);
        }
        if (size <= 0) {
            break;
        }
    }
    printf("%s %s here\n", buff, filename);
    fclose(fp);
    close(server_fd);
}
int recv_file () {
    int port, server_listen, server_fd, pid;
    port = 8736;
    char buff[BUFFSIZE] = {0};
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    }
    int on = 1;
    setsockopt(server_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        if ((server_fd = accept(server_listen, (struct sockaddr *)&client_addr, &len)) < 0 ){
        exit(1);
    }
        pthread_t p;
        struct date tmp;
        tmp.sockfd  = server_fd;
        strcpy(tmp.ip, inet_ntoa(client_addr.sin_addr));
        pthread_create(&p, NULL, _recv_file, (void *)&tmp);
    }
    close(server_listen);
    return 0;
}
