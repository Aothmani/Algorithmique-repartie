#include <mpi.h>
#include <stdio.h>
#include "mpi_server.h"

char buf[100];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_msg_recu = PTHREAD_COND_INITIALIZER;

void attenton(int tag, int source){
  MPI_Status status;
  MPI_Recv(buf, 20, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
  pthread_mutex_lock(&mutex);
  pthread_cond_broadcast(&cond_msg_recu);
  pthread_mutex_unlock(&mutex);
}

int main( int argc, char** argv){
  int rank;
  int size;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, NULL);
  printf("totototototototototo");
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  start_server(attenton);
  
  sprintf(buf, "Hello %d from %d", (rank+1)%size, rank);
   
  MPI_Send(buf,20, MPI_CHAR, (rank+1)%size, 0,MPI_COMM_WORLD);
  pthread_mutex_lock(&mutex);
  pthread_cond_wait(&cond_msg_recu, &mutex);
  pthread_mutex_unlock(&mutex);
  printf("%s\n",buf);

  destroy_server();
  
  MPI_Finalize();
  return 0;
}
