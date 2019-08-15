/**************************************************************************************
	> File Name: log.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月06日 星期二 16时33分31秒
 *************************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<syslog.h>

int main(){
    int i;
    openlog("tiger-error:",LOG_CONS|LOG_PID,LOG_LOCAL0);
    for(i = 0;i < 3;i++){
        syslog(LOG_ERR,"Error%dth\n",i);
    }
    closelog();
    openlog("tiger-warning:",LOG_CONS|LOG_PID,LOG_LOCAL1);
    for(i = 0;i < 2;i++){
        syslog(LOG_WARNING,"Warning%dth\n",i);
    }
    closelog();
    return 0;
}
