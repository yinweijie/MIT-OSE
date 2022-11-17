#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *file_name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type != T_DIR) {
    printf("Usage: find [path] [file_name]\n");
    exit(0);
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    printf("ls: path too long\n");
  }
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
  // now, buf = "./"
  // p is appended to the end of "./"
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    // now buf = "./.", "./..", "./a", "./b" , "./c" ... in each while loop
    // p = ".", "..", "a", "b", "c" ...
    if(stat(buf, &st) < 0){
      printf("ls: cannot stat %s\n", buf);
      continue;
    }

    if(st.type == T_FILE && (strcmp(file_name, p) == 0)) {
      printf("%s\n", buf);
    } else if(st.type == T_DIR && (strcmp(p, ".") != 0) && (strcmp(p, "..") != 0)) {
      find(buf, file_name);
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf("Usage: find [path] [file_name]\n");
    exit(0);
  }

  // eg. find . b
  find(argv[1], argv[2]);
  exit(0);
}
