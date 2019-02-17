#include <mpi.h>
#include <stdio.h>
#include "mpi_server.h"




int main( int argc, char** argv){
  int rang;
  int size;
  
  
  MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  
  
  printf("Processus %d sur %d : Hello MPI \n", rang, size);
  MPI_Finalize();
  return 0;
}
