/*************************************************************************
	> File Name: log2.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月06日 星期二 20时52分16秒
 ************************************************************************/

#include <stdio.h>
#include <syslog.h>
int main(int argc, char **argv)
{
    openlog("MyMsgMARK", LOG_CONS | LOG_PID, 0);
    syslog(LOG_INFO,
           "This is a syslog test message generated by program '%s'\n",
           argv[0]);
    closelog();
    return 0;
}
