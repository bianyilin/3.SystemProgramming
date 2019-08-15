/*************************************************************************
	> File Name: master.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月01日 星期四 09时47分10秒
 ************************************************************************/

#include "common.h"

typedef struct Node {
    int fd;
    char ip[20];
    char name[20];
    struct Node *next;
} Node, *List;

List list;
int cnt = 0;
int timeout = 1;

void output(List list);

void insert(List list, Node *node) {
    Node *p = list;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = node;
    cnt++;
    return ;
}
void insertl(List list, Node *node) {
    Node *p = list;
    while (p->next != NULL && strcmp(p->next->ip, node->ip) != 0) {
        p = p->next;
    }
    output(list);
    if (p->next != NULL && strcmp(p->next->ip, node->ip) == 0) {
        return ;
    }
    p->next = node;
    printf("\033[;33;5m%s已上线\033[0m\n", p->next->ip);
    cnt++;
    return ;
}

void delete (List list, Node *node) {
    Node *p, *q;
    p = list;
    while (p->next != NULL && p->next != node) {
        p = p->next;
    }
    if (p->next == NULL) return ;
    q = p->next;
    p->next = p->next->next;
    free(q);
    cnt--;
    return ;
}

List create_node(int fd, char *name, char *ip) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->next = NULL;
    node->fd = fd;
    strcpy(node->ip, ip);
    strcpy(node->name, name);
    return node;
}

void output(List list) {
    Node *p;
    p = list;
    int flag = 0;
    while (p->next != NULL) {
        flag++;
        printf("%s:5679 \033[32monline\033[0m ... \n", p->next->ip);
        p = p->next;
    }
    if (!flag) {
        printf("❤ \n");
    }
    return ;
}

int check_connect(char *ip, long timeout){
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
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
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5679);
    addr.sin_addr.s_addr = inet_addr(ip);

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


void *heart_jump() {
    for (int i = 1; i >= 1; i--) {
            Node *p = list;
            while (p->next != NULL) {
                if (!check_connect(p->next->ip, timeout)) {
                    printf("%s:5679 \033[31mdeleting\033[0m ... \n", p->next->ip);
                    if (i == 1) {
                        Node *q = p->next;
                        delete(list,q);
                    } else { 
                        p = p->next;
                    }
                } else {
                    printf("%s:5679 \033[32monline\033[0m ... \n", p->next->ip);
                    p = p->next;
                }
            }
        sleep(2);
    }
    Node *p = list;
    char heart = '*';
    int flag = 1;
    while (1) {
        p = list;
        while (p->next != NULL) {
            if ((send(p->next->fd, &heart, 1, MSG_NOSIGNAL) < 0) && (!check_connect(p->next->ip, timeout))) {
                Node *node = p->next;
                printf("\033[;33;5m%s已下线\033[0m\n", p->next->ip);
                delete(list, node);
                break;
            } else {
               p = p->next;
            }
        }
        printf("\033[;36;5m当前在线用户%d人:\033[0m\n", cnt);
        output(list);
        for (int i = 0; i < cnt; i++) {
            printf("client: Hello Haizei\n");
        }
        flag++;
        sleep(2);
    }
    return NULL; 
}


int main() {
    char ports[10] = {0};
    get_conf_val("ma.conf", "master_port", ports);
    int port = ctoi(ports);
	char tmp[20] = {0};
    char FromIp[20] = {0};
    char ToIp[20] = {0};
    get_conf_val("ma.conf", "From", tmp);
    strcpy(FromIp, tmp);
    get_conf_val("ma.conf", "To", tmp);
    strcpy(ToIp, tmp); 
    Node *node = (Node *)malloc(sizeof(Node));
    node->next = NULL;
    node->fd = 0;
    strcpy(node->ip, "init");
    strcpy(node->name, "init");
    list = node;
    unsigned int begin, end;
    begin = ntohl(inet_addr(FromIp));
    end = ntohl(inet_addr(ToIp));
    struct sockaddr_in initaddr;
    for (unsigned int i = begin; i <= end; i++) {
        if (i % 256 == 0 || ((i + 1) % 256 == 0)) {
            continue;
        }
        initaddr.sin_addr.s_addr = htonl(i);
        Node *p = (Node *)malloc(sizeof(Node));
        strcpy(p->ip , inet_ntoa(initaddr.sin_addr));
        p->next = NULL;
        strcpy(p->name, "weizhi");
        p->fd = -1;
        insert(list, p);
    }
    pthread_t Heart_Jump;
    pthread_create(&Heart_Jump, NULL, heart_jump, NULL);
    sleep(2);

    int master_listen, sockfd;  
    char ip[20], name[20];
    
    struct sockaddr_in my_addr, peer_addr;
    if ((master_listen = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    }
 
    socklen_t len = sizeof(struct sockaddr);
    while (1) {
        if ((sockfd = accept(master_listen, (struct sockaddr *)&peer_addr, &len)) < 0) {
            perror("accept");
            return 1;
        }
        recv(sockfd, name, sizeof(name), 0);
        strcpy(ip, inet_ntoa(peer_addr.sin_addr));
        
        Node *node = create_node(sockfd, name, ip);
        insertl(list, node);
        memset(name, 0, sizeof(name));
        memset(ip, 0, sizeof(ip));
    }

    return 0;
}

