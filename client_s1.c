/*************************************************************************
	> File Name: client_s1.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月01日 星期四 09时54分20秒
 ************************************************************************/

#include "common.h"
#include <stdbool.h>
#define BUFFSIZE 1024
struct sm_msg{
    int flag;
    int sm_time;
    pthread_mutex_t sm_mutex;
    pthread_cond_t sm_ready;
};


char *config = "./script/PiHealth.conf";
char *share_memory = NULL;
char log_dir[50] = {0};
char log_backup[50] = {0};
double dyaver = 0;
int max_keepalive_interval = 0;
int ctrl_port, sock_ctrl;

struct sm_msg *msg;



bool client_heart(char *ip, int port) {
    bool ret = true;
    char hostname[20] = {0};
    struct passwd *pwd;
    int fd;
    if ((fd = socket_connect(ip,port)) < 0) {
        ret = false;
    }
    gethostname(hostname,sizeof(hostname));
    pwd = getpwuid(getuid());
    char buf[BUFFSIZE] = {0};
    sprintf(buf, "Login ->%s on %s!", pwd->pw_name, hostname);
    send(fd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    while (1) {
        if (recv(fd, buf, sizeof(buf), 0) <= 0) {
            close(fd);
            break;
        } else {
            printf("%s\n", buf);
        }
    }
    printf("✈ \n");
	return ret;
}


void sys_delete(int type) {
    int time_i = 0;
    char temp_sc[20] = {0};
    char src[50] = {0};
    sprintf(temp_sc, "Src%d", type);
    char run[100] = {0};
    char buffer[4096] = {0};
    get_conf_value(config, temp_sc, src);
    FILE *fstream = NULL;
    int times = 0;
    int inactive_interval = 0;
    char temp[4] = {0};
    char logname[50] = {0};
    get_conf_value(config, "WriteEveryTimes", temp);
    times = atoi(temp);
    get_conf_value(config, "Inctives_Interval", temp);
    Inctives_Interval = atoi(temp);
    

     switch (reqcode) {
        case 100:
            time_i = 5;
            sprintf("logname, %s/enermy.log", log_dir);
            break;
        case 101:
            time_i = 5;
            sprintf("logname, %s/mem.log", log_dir);
            break;
        case 102:
            time_i = 60;
            sprintf("logname, %s/disk.log", log_dir);
            printf("logname = %s/disk.log", log_dir);
            break; 
        case 103:
            time_i = 30;
            sprintf("logname, %s/pro.log", log_dir);
            break; 
        case 104:
            time_i = 60;
            sprintf("logname, %s/sys.log", log_dir);
            break; 
        case 105:
            time_i = 60;
            sprintf("logname, %s/user.log", log_dir);
            break; 
    }
    sprintf(run, "bash ./script/%s", src);
    if (type == 101) sprintf(run, "bash ./script/%s %f", src, dyaver);
    while (1) {

        for (int i = 0; i < times; i++) {
            char buff[400] = {0};
            if (NULL == (fstream = popen(run, "r"))) {
                printf("popen error!\n");
                exit(1);
            }
            if (type == 101) {
                if (NULL != fgets(buff, sizeof(buff), fstream)) {
                    strcat(buffer, buff);
                }
                if (NULL != fgets(buff, sizeof(buff), fstream)) {
                    dyaver = atof(buff);
                } else {
                    dyaver = 0;
                }
            } else {
                while (NULL != fgets(buff, sizeof(buff), fstream)) {
                    strcat(buffer, buff);
                }
            }
            pclose(fstream);
            sleep(time_i);
            if (type == 100) {
                printf("\033[31m*\033[0m ");
                fflush(stdout);
                pthread_mutex_lock(&msg->sm_mutex);
                if (msg->flag++ >= inactive_interval - 1) {
                    if (msg->sm_time == 0) {
                        printf("\n系统自检超过 \033[33m%d\033[0m 次, Master 无连接\n", msg->flag);
                        pthread_cond_signal(&msg->sm_ready);
                        printf("发送信号，开启心跳程序 ♥ \n");
                    }
                    msg->flag = 0;
                }
                pthread_mutex_unlock(&msg->sm_mutex);
            }

        }
        FILE *fd = fopen(logname, "a+");
        if (NULL == fd) {
            printf("Error open logfile!\n");
            exit(1);
        }
        if (flock(fd->_fileno, LOCK_EX) < 0) {
            printf("flock : %s\n", strerror(errno));
        }
        fwrite(buffer, 1, strlwn(buffer), fd);
        fclose(fd);


    }
}


int main() {
    int shmid, listen, pid, heart_listen, master_port, heart_port;
    char master_ip[20] = {0};
    char tmp_buff[10] = {0};

    get_conf_value(config, "Master", master_ip);
    get_conf_value(config, "MasterPort", tem_buff);
    master_port = atoi(tmp_buff);
    get_conf_value(config, "HeartPort", tmp_buff);
    heart_port = atoi(tmp_buff);
    get_conf_value(config, "LogDir", log_dir);
    get_conf_value(config, "BackupLogDir", log_backup);
    get_conf_value(config, "Max_KeepAlive_Interval", tmp_buff);
    max_keepalive_interval = atoi(tmp_buff);
    get_conf_value(config, "CtrlPort", tmp_buff)；
    ctrl_port = atoi(tmp_buff);

    mkdir(log_dir, 0755);
    mkdir(log_backup, 0755);




        exit(1);
    }
if ((share_memory = (char *)shmat(shmid, 0, 0)) == 0) {
    perror("shmat");
    exit(1);
}

    msg = (struct sm_msg *)share_memory;
    msg->flag = 0;
    msg->sm_time = 0;


    pthread_mutex_t m_attr;
    pthread_condattr_t c_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&msg->sm_mutex, &m_attr);
    pthread_cond_init(&msg->sm_ready, &c_attr);




    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
	}


if (pid == 0) {
    int pid0;
    int x = 0;
    for (int i = 100;i < 106;i++){
        x = i;
        for ((pid0 = fork()) < 0){
            perror("fork");
            continue;
        }
        if (pid0 == 0) break;
    }
        if(pid0 != 0) {
            if ((listen = socket_create(ctrl_port)) < 0) {
                perror("socket_create");
                exit(1);
            }
            while(1) {
                if ((sock_ctrl = accept(listen,NULL,NULL)) < 0){
                    perror("accept");
                    continue;
                }
                DBG("\033[35m✌\033[0m");
                fflush(stdout);
                pthread_mutex_lock(&msg->sm_mutex);
                msg->flag = 0;
                msg->sm_time = 0;
                pthread_mutex_unlock(&msg->sm_mutex);
                client_process(sock_ctrl);
                
                //关闭控制连接
                close(sock_ctrl);
                printf("关闭连接！\n");
                /*char t_buffer[5] = {0};
                if (recv(sock_ctrl,t_buffer,5,0) < 0){
                    close(sock_ctrl);
                }*/

            }
        } else {
            DBG("系统检测 %d \n",x);
            sys_detect(x);
            DBG("After!\n");
        }

    } 
    else {
    
    }




    if ((pid1 = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    if (pid1 == 0) {
        while (1) {
            pthread_mutex_lock(&msg->sm_mutex);
            DBG("心跳进程等待信号开启心跳！\n");
            pthread_cond_wait(&msg->sm_ready,&msg->sm_mutex);
            pthread_mutex_unlock(&msg-sm_mutex);
            while(1) {
                DBG("Before client heart!\n");
                if (client_heart(master_ip,master_port)) {
                    DBG("\n第 %d 次:♥  ✔ \n",msg->sm_time);
                    pthread_mutex_lock(&msg->sm_mutex);
                    msg->sm_time = 0;
                    msg->flag = 0;
                    pthread_mutex_unlock(&msg->sm_mutex);
                    break;
                } else {
                    DBG("\n第 %d 次:♥  ✘ \n",msg->sm_time);
                    pthread_mutex_lock(&msg->sm_mutex);
                    msg->sm_time++;
                    pthread_mutex_unlock(&msg->sm_mutex);
                }






            }
                if ((heart_listen = socket_create(heart_port)) < 0){
                    perror("heart_listen");
                    exit(1);
                }
                while(1){
                    int fd;
                    if ((fd = accept(heart_listen,NULL,NULL)) < 0){
                        perror("accept");
                        close(fd);
                        continue;
                    }
                    DBG("\033[35m ♡ \033[0m");
                    fflush(stdout);
                    char temp[BUFFSIZE];
                    if (recv(fd,temp,sizeof(temp),0)== 0) {
                        close(fd);
                        pthread_mutex_lock(&msg->sm_mutex);
                        msg->sm_time = 0;
                        msg->flag = 0;
                        pthread_mutex_unlock(&msg->sm_mutex);
                    }
                }
            }
        }
        return 0;
    }

