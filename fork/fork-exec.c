#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int spawn (char* program, char** arg_lsit){
  pid_t child_pid;

  child_pid = fork();
  if (child_pid){
    return child_pid;
  }
  else {
    execvp(program,arg_lsit);
    fprintf(stderr, "an error occurred\n" );
    abort();
  }
}
int main(){
  char* arg_list[] = {
    "ls",
    "-l",
    "/",
    NULL
  };
  spawn("ls",arg_list);
  return 0;
}
