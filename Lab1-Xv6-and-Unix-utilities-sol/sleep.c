#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc <= 1){
    fprintf(2, "usage: sleep [time]\n");
    exit(1);
  }

  int sleep_time = atoi(argv[1]);

  sleep(sleep_time);

  exit(0);
}
