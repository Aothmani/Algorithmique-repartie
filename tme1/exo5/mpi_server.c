#include "mpi_server.h"
#include <pthread.h>

static server the_server;
int run = 1;

void *proc_listener(){
  MPI_Status status;
  int flag = 0;
    
    while(run){
      pthread_mutex_lock(&the_server.mutex);
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      pthread_mutex_unlock(&the_server.mutex);
      if(flag){
	the_server.callback(status.MPI_TAG, status.MPI_SOURCE);
      }
      flag = 0;
    }
}


void start_server(void (*callback)(int tag, int source)){
  the_server.callback = callback;
  pthread_mutex_init(&the_server.mutex, NULL);
  pthread_create(&the_server.listener, NULL, proc_listener, NULL);
}


void destroy_server(){
  run = 0;
  pthread_join(the_server.listener, NULL);
}


pthread_mutex_t* getMutex(){
    return &the_server.mutex;
}


