/*************************************************************************
	> File Name: client_h.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月01日 星期四 09时51分18秒
 ************************************************************************/

#include "common.h"
#define BUFFSIZE  1024
struct  Share  {
    int  share_cnt;
    pthread_mutex_t  smutex;
    pthread_cond_t  sready;  
};

struct  Script  {
    char  *logpath;
    char  message[BUFFSIZE*5];
};

char  check_info[6][BUFFSIZE*5];

pthread_mutexattr_t  mattr;
pthread_condattr_t  cattr;

void  do_con(char  *ip,  int  port)  {
    int  retime  =  2;
    while  (1)  {
        int  sockfd  =  socket_connect(ip,port);
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

void  recv_heart(int  port,  struct  Share  *msg)  {
    int  sockfd  =  socket_create(port);
    if  (sockfd  <  0)  {
        perror("Error  bind  on  heartPort");
        return  ;
    }
    while  (1)  {
        int  newfd  =  accept(sockfd,  NULL,  NULL)  ;
        pthread_mutex_lock(&msg->smutex);
        msg->share_cnt  =  0;
        pthread_mutex_unlock(&msg->smutex);
        printf("  ❤    ");
        fflush(stdout);
        close(newfd);
    }  
    close(sockfd);
}

void  recv_data(int  ctlPort,  struct  Share  *msg)  {
        int  listenfd;
        if  ((listenfd  =  socket_create(ctlPort))  <  0)  {
                perror("socket_create  on  listenfd");
                exit(1);
        }
        while  (1)  {
                int  newfd;
                if  ((newfd  =  accept(listenfd,  NULL,  NULL))  <  0)  {
                        perror("accept");
                        continue;
                }
                char  buff[BUFFSIZE]  =  {0};
                int  k  =  recv(newfd,  buff,  BUFFSIZE,  0);
                if  (k  <  0)  {
                        perror("recv");
                        close(newfd);
                        continue;
                }
                printf("%s\n",  buff);
                strcpy(buff,  "nice  day\n");
                send(newfd,  buff,  strlen(buff),  0);
                pthread_mutex_lock(&msg->smutex);
                msg->share_cnt  =  0;
                pthread_mutex_unlock(&msg->smutex);
                close(newfd);
        }
        close(listenfd);
}

int  do_check_info(char  *bsname,  char  *destfile,  int  cnt,  int  ind)  {
        FILE  *pfile  =  NULL,  *fp;
        char  filename[100]  =  {0};
        sprintf(filename,  "bash  %s",  bsname);
        pfile  =  popen(filename,  "r");
        fp  =  fopen(destfile,  "a+");
        if  (!pfile)  {
                fprintf(fp,  "Error  :  Script  run  failed\n");
                return  0;
        }
        char  buff[BUFFSIZE];
        while(fgets(buff,  BUFFSIZE,  pfile)  !=  NULL)  {
                strcat(check_info[ind],  buff);
        }
        if  (cnt  ==  5)  {
                fprintf(fp,"%s",  check_info[ind]);
                memset(check_info[ind],  0,  sizeof(check_info[ind]));
        }
        fclose(fp);
        pclose(pfile);
        return  1;
}

void  do_check(struct  Share  *msg,  int  cnt)  {
        int  flag;
        flag  =  do_check_info("cpu.sh",  "./cpu.log",  cnt,  0);  
        flag  &=  do_check_info("disk.sh",  "./disk.log",  cnt,  1);  
        flag  &=  do_check_info("Memlog.sh  24",  "./mem.log",  cnt,  2);  
        flag  &=  do_check_info("user.sh",  "./user.log",  cnt,  3);  
        flag  &=  do_check_info("script.sh",  "./scr.log",  cnt,  4);  
       // flag  &=  do_check_info("prolog.sh", "./prolog.log", cnt, 5); 
   
    if (flag == 0) {
        printf("check failed\n");
        return ;
    } else {
        printf("check success\n");
    }
    sleep(2);
    
    if (msg->share_cnt >= 5) return ;

    pthread_mutex_lock(&msg->smutex);
    msg->share_cnt += 1;
    if (msg->share_cnt >= 5) {
        pthread_cond_signal(&msg->sready);
    }
    pthread_mutex_unlock(&msg->smutex);
}

int main() {
    int heartPort, ctlPort, loadPort;
    char tmp[20]={0};
    char *config = "./ma.conf.log";
    char ip[20]={0};
    
    get_conf_val(config, "HeartPort", tmp);
    heartPort = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_val(config, "IP", tmp);
    strcpy(ip, tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_val(config, "CtlPort", tmp);
    ctlPort = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_val(config, "LoadPort", tmp);
    loadPort = atoi(tmp);
    printf("HeartPort : %d\nCtlPort : %d\nLoadPort : %d\n", heartPort, ctlPort, loadPort);

    struct Share *share_msg = NULL;
    pthread_mutexattr_init(&mattr);
    pthread_condattr_init(&cattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    
    int shmid = shmget(IPC_PRIVATE, sizeof(struct Share), IPC_CREAT | 0666);
    if (shmid < 0) {
        printf("共享内存申请失败\n");
        exit(1);
    }
    share_msg = (struct Share*) shmat (shmid, NULL, 0); 
    if (share_msg == (void*)-1) {
        printf("获取共享内存地址失败\n");
        exit(1);
    }
    pthread_mutex_init(&(share_msg->smutex), &mattr);
    pthread_cond_init(&(share_msg->sready), &cattr);
    
    pid_t pid = fork();
    if (pid > 0) {
        recv_data(ctlPort, share_msg);
        shmdt((void*)share_msg);
        wait(&pid);
    }else if (pid == 0) {
        pid_t npid = fork();
        if (npid > 0) {  
            recv_heart(heartPort, share_msg);
            wait(&npid);
            exit(0);
        }
        if (npid == 0) {
            pid_t mpid = fork();
            if (mpid > 0) {
                while (1) {
                    pthread_mutex_lock(&share_msg->smutex);
                    pthread_cond_wait(&share_msg->sready, &share_msg->smutex);
                    do_con(ip, loadPort);
                    share_msg->share_cnt = 0;
                    pthread_mutex_unlock(&share_msg->smutex);
                }
                wait(&mpid);
                exit(0);
            } else if (mpid == 0) {
                int cnt = 0;
                while (1) {
                    cnt ++;
                    do_check(share_msg, cnt);
                    if(cnt >= 5) cnt = 0;
                }
                exit(0);
            }
        }
    }
    return 0;
}

