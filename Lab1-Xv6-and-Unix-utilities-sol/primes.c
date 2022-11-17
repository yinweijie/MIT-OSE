#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
pipe_line(int left_fds)
{
/*
p = get a number from left neighbor
print p
loop:
    n = get a number from left neighbor
    if (p does not divide n)
        send n to right neighbor
*/

  int p;
  int n;
  n = read(left_fds, &p, sizeof(p)); // read from left neighbor

  if(p == -1) {
    exit(0);
  }

  if(n > 0) printf("prime %d\n", p);

  // if(p == 7) {
  //   exit(0);
  //   // return;
  // }

  int fds[2];
  int pid;
  int buf;

  pipe(fds);

  pid = fork();

  if(pid == 0) { // child
    close(left_fds);
    close(fds[1]);
    pipe_line(fds[0]);
    // read(fds[0], &buf, sizeof(buf));
    // printf("prime %d\n", buf);
  } else if(pid > 0) {
    close(fds[0]);
    // read from left neighbor
    while((read(left_fds, &buf, sizeof(buf)) > 0) && (buf != -1)) {
      // printf("-- %d\n", buf);
      if(buf % p != 0) { // buf is a prime
        write(fds[1], &buf, sizeof(buf)); // send to right neighbor
      }
    }

    buf = -1;
    write(fds[1], &buf, sizeof(buf)); // add -1 to the last one
    
    close(left_fds);
    close(fds[1]);

    wait((int*)0);
  } else {
    printf("fork error\n");
  }

  exit(0);
}

int
main(int argc, char *argv[])
{
  int arr[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
               12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
               22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
               32, 33, 34, 35};
  // int arr[] = {2, 3, 4, 5, 6, 7, 8};

  int fds[2];
  int pid;

  pipe(fds);

  pid = fork();

  if(pid == 0) { // child
    close(fds[1]);
    pipe_line(fds[0]);

  } else if(pid > 0) {
    close(fds[0]);

    for(int i = 0; i < (sizeof(arr) / sizeof(int)); i++) {
      write(fds[1], &arr[i], sizeof(int)); // send to right neighbor
    }
    
    close(fds[1]);

    wait((int*)0);
  } else {
    printf("fork error\n");
  }

  exit(0);
}
