/*************************************************************************
	> File Name: zy2.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年04月23日 星期二 20时41分54秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

int main(void) {
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    char hostname[32 + 1];
    gethostname(hostname, sizeof(hostname));
    char buf[80];
    getcwd(buf,sizeof(buf));
    printf("\033[34m%s@%s:%s\033[1m\n",pwd->pw_name,hostname,buf);
    return 0;
}
