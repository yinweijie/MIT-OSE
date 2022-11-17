#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

char buf[512];

char*
gets1(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    
    if(c == '\\') {
      cc = read(0, &c, 1);
      if(cc < 1)
        break;

      if(c == 'n') {
        c = '\n';
      }
    }

    // noted that '\n' is passed by stdin, however command should not include it
    if(c == '\n' || c == '\r') {
      buf[i++] = '\0';
      break;
    } else {
      buf[i++] = c;
    }
  }

  return buf;
}

int
getcmd(char *buf, int nbuf)
{
  memset(buf, 0, nbuf);
  gets1(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
append_argvv(char* argvv[], int cc, char buf[])
{
  argvv[cc++] = buf;
  char* q;

  // "aaa bbb\0"
  while((q = strchr(buf, ' ')) != 0){
    *q = '\0';
    q++;
    argvv[cc++] = q;
  }

  return cc;
}

int
main(int argc, char *argv[])
{
  static char buf[100];
  // $ find . b | xargs grep hello
  char* argvv[MAXARG]; // argvv[0] = grep, argvv[1] = hello, argvv[2] = ... from "find . b"

  // argv = "xargs grep hello"
  // argvv = "grep hello ..."
  for(int i = 1; i < argc; i++) {
    argvv[i - 1] = argv[i];
  }

  int cc = argc - 1;
  int ee = 0;
  // read from fd = 0; fd = 0 has be relocated to pipe[0]
  while(getcmd(buf, sizeof(buf)) >= 0){

    /// if "find . b" output: "aaa bbb\nccc ddd"
    /// in each loop, *exec* should execute:
    /// grep hello aaa bbb, grep hello ccc ddd
    /// buf = "aaa bbb\0", buf = "ccc ddd\0"

    // argvv[2] = "aaa", argvv[3] = "bbb"
    ee = append_argvv(argvv, cc, buf);
    argvv[ee] = 0; // argvv[4] = 0, end of command, don't forget it

    if(fork() == 0) {
      exec(argvv[0], argvv);
    }

    wait(0);
  }

  exit(0);
}
