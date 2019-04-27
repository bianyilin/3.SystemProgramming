/*************************************************************************
	> File Name: test.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年04月27日 星期六 18时48分40秒
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    struct stat *st = malloc(sizeof(struct stat));
    if (stat(argv[1], st) < 0) {
        perror("stat");
    }
    printf("%d\t%d\t%d\t%d\t%d\t\t%s\n", st->st_mode, st->st_nlink, st->st_uid, st->st_gid, st->st_size, argv[1]);
    return 0;
}

