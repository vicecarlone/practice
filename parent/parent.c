#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

void spawn (int num, pid_t* pid){
  if (!num) return;
  pid_t child;
  child = fork();
  if (!child){
    printf("this is the %i process, n = %i and the parent is: %i\n", getpid(),num,getppid());
    exit(0);
    return;
  }
  else {
    pid[0] = child;
    spawn(num-1,pid);
  }
}


int main(int argc, char *argv[]){


  //read input and process the input value
  //if there is no parameter, nothing happen but the printf
  if (argc == 1) {
    printf("there is no child process\n");
    return 0;
  }
  pid_t parent = getpid();
  int num_child = argv[1][0] - '0';
  printf("the number of child processes:%c\n", (int)argv[1][0]);
  printf("parent processes %i\n",getpid() );


  //create the shared memory for processes.
  const long unsigned int size_pid = sizeof(pid_t);
  int segment_id;
  pid_t* shared_memory;
  struct shmid_ds shmbuffer;
  int segment_size;
  const int shared_segment_size = size_pid * num_child;
  /*allocate the shared memory*/
  segment_id = shmget (IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  /* Attach the shared memory segment. */
  shared_memory = (pid_t*) shmat (segment_id, 0, 0);
  printf ("shared memory attached at address %p\n", shared_memory);
  /* Determine the segment’s size. */
  shmctl (segment_id, IPC_STAT, &shmbuffer);
  segment_size = shmbuffer.shm_segsz;
  printf ("segment size: %d\n", segment_size);





  //intiate the children processes
  for (int i = 0; i < num_child; i++){
    pid_t child = fork();
    if (!child){
      printf("this is the %i process, n = %i and the parent is: %i\n", getpid(),i,getppid());
      shared_memory[i] = getpid();
      break;
    }
  }

  //print the list children process from the shared_memory
  if (parent == getpid()){
    int status;
    wait(&status);
    //pause the parent for 2s to prevent zombie process
    sleep(2);
    printf("the list of children pid:\n");
    for (int i = 0; i < num_child; i++){
      printf("%i\n", shared_memory[i]);
    }

    /* Detach the shared memory segment. */
    shmdt (shared_memory);
    /* Deallocate the shared memory segment. */
    shmctl (segment_id, IPC_RMID, 0);
  }
  return 0;
}
