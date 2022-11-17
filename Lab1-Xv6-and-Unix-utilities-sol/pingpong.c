#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fds1[2], fds2[2];
  int pid;
  char ping_pong[1];

  pipe(fds1);
  pipe(fds2);

  pid = fork();

  if(pid == 0) {
    close(fds1[1]);
    close(fds2[0]);

    read(fds1[0], ping_pong, sizeof(ping_pong));
    printf("%d: received ping\n", getpid());
    write(fds2[1], ping_pong, sizeof(ping_pong));

    close(fds1[0]);
    close(fds2[1]);
  } else if(pid > 0) {
    close(fds1[0]);
    close(fds2[1]);

    ping_pong[0] = 'P';
    write(fds1[1], ping_pong, sizeof(ping_pong));
    read(fds2[0], ping_pong, sizeof(ping_pong));
    printf("%d: received pong\n", getpid());

    close(fds1[1]);
    close(fds2[0]);
  } else {
    printf("fork error\n");
  }

  exit(0);
}
