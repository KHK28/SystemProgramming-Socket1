#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{

struct timespec ts;
timespec_get(&ts, TIME_UTC);
printf(".%09ld\n", ts.tv_nsec);
printf(".%03ld\n", ts.tv_nsec/1000000);
return 0;

}
