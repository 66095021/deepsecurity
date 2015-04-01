#include <stdio.h>

#define err_log(fmt, ...) {  \
FILE * fp = fopen("/tmp/test.txt", "a");\
fprintf(fp, fmt, ##__VA_ARGS__);\
fclose(fp);\
}

main()
{
err_log("can");
err_log("ccccc");
char *p ="foo";
err_log("%s \n",p );
}
