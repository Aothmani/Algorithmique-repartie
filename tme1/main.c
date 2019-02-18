#include <mpi.h>
#include <stdio.h>
#include "mpi_server.h"

char buf[100];

char* receive(int tag, int source){
  int status;
  buf = MPI_Receive(buf, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
  return status;
}

int main( int argc, char** argv){
  int rang;
  int size;
    
  MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  sprintf(message, "Hello %d from %d", (rank+1)%size, rank);
    
  if (rank != size-1){
    MPI_Send(message, strlen(message)+1, MPI_CHAR, (rank+1)%size, 0,MPI_COMM_WORLD);
    MPI_Recv(message, BSIZE, MPI_CHAR, (rank-1)%size, 0, MPI_COMM_WORLD, &status);
    printf("%s\n",message);
  }else{
    MPI_Recv(msg, BSIZE, MPI_CHAR, (rank-1)%size, 0, MPI_COMM_WORLD, &status);
    printf("%s\n",msg);
    MPI_Send(message, strlen(message)+1, MPI_CHAR, (rank+1)%size, 0, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
  return 0;
}
