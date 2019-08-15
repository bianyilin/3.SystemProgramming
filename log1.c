/*************************************************************************
	> File Name: log1.c
	> Author: bianyilin
	> Mail: 732406982@qq.com 
	> Created Time: 2019年08月06日 星期二 19时52分18秒
 ************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
 
int write_log (FILE* pFile, const char *format, ...) {
	va_list arg;
	int done;
 
	va_start (arg, format);
 
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	fprintf(pFile, "%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
 
	done = vfprintf (pFile, format, arg);
	va_end (arg);
 
	fflush(pFile);
    fclose(pFile);
    return done;
}
 
int main() {
	FILE* pFile = fopen("log.txt", "a");
	write_log(pFile, "%s %d %f\n", "is running", 10, 55.55);
	fclose(pFile);
 
	return 0;
}

