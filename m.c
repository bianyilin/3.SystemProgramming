/*************************************************************************
	> File Name: m.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月11日 星期日 17时55分11秒
 ************************************************************************/

#include<stdio.h>
#include "master.h"

int main() {
    pid_t p;
    p = fork();
    if (p) exit(0);
    char *config = "./healthd.conf";
    int server_listen;
    int Port, Ins, CHPort;
    long TimeOut;
    char FromIp[20] = {0};
    char ToIp[20] = {0};
    pthread_t HeartTid, t[Ins];
    char tmp[20] = {0};
    char temp[BUFFSIZE];
    get_conf_value(config, "INS", tmp);
    Ins = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "From", tmp);
    strcpy(FromIp, tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "To", tmp);
    strcpy(ToIp, tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "ClientHeart", tmp);
    CHPort = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "MasterPort", tmp);
    Port = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "TimeOut", tmp);
    TimeOut = atol(tmp);
    sprintf(temp, "CHPort:%d\n Port:%d\n", CHPort, Port);
    write_log(LOGFILE, "%s", temp);

    unsigned int begin, end;
    begin = ntohl(inet_addr(FromIp));
    end = ntohl(inet_addr(ToIp));

    LinkedList *linkedlist = (LinkedList *)malloc(Ins * sizeof(LinkedList));
    int *sum = (int *)malloc(Ins * sizeof(int));
    memset(sum, 0, Ins * sizeof(int));

    struct sockaddr_in initaddr;
    initaddr.sin_family = AF_INET;
    initaddr.sin_port = htons(CHPort);
    initaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    for (int i = 0; i < Ins; i++) {
        Node *p = (Node *)malloc(sizeof(Node));
        p->client_addr = initaddr;
        p->node_sock = -1;
        p->next = NULL;
        linkedlist[i] = p;
    }

    for (unsigned int i = begin; i <= end; i++) {

        if (i % 256 == 0 || ((i + 1) % 256 == 0)) {
            continue;
        }
        initaddr.sin_addr.s_addr = htonl(i);
        Node *p = (Node *)malloc(sizeof(Node));
        p->client_addr = initaddr;
        p->next = NULL;
        p->node_sock = -1;
        int sub = find_min(sum, Ins);
        insert(linkedlist[sub], p);
        sum[sub]++;
    }

    for (int i = 0; i < Ins; i++) {
        sprintf(temp, "Ins <%d>:\n", i);
        write_log(LOGFILE, "%s", temp);
        print_linkedlist(linkedlist[i]);
    }

    struct HEART heartarg;
    heartarg.sum = sum;
    heartarg.head = linkedlist;
    heartarg.ins = Ins;
    heartarg.timeout = TimeOut;

    if (pthread_create(&HeartTid, NULL, heartbeat_all, (void *)&heartarg) < 0) {
        PERROR("pthread_create");
        write_log(LOGFILE, "%s", strerror(errno));
        exit(1);
    }

    struct WORK workarg[Ins];


    sleep(3);

    for (int i = 0; i < Ins; i++) {
        workarg[i].head = linkedlist[i];
        workarg[i].index = i;
        if (pthread_create(&t[i], NULL, work, (void *)&workarg[i]) < 0) {
            PERROR("pthread_create");
            write_log(LOGFILE, "%s", strerror(errno));
            exit(1);
        }
    }
    if ((server_listen = socket_create(Port)) < 0) {
        PERROR("server_listen");
        write_log(LOGFILE, "%s", strerror(errno));
        exit(1);
    }
    do_epoll(server_listen, linkedlist, sum, Ins, CHPort);

    for (int i = 0; i < Ins; i++) {
        pthread_join(t[i], NULL);
    }
    pthread_join(HeartTid, NULL);

    return 0;
}


