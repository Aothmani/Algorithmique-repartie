#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define BSIZE 128

int main( int argc, char** argv){
    int rank;
    int size;
    char message[BSIZE];
    char msg[BSIZE];
    
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    sprintf(message, "Hello %d from %d", (rank+1)%size, rank);
    
    if (rank != size-1){
      MPI_Ssend(message, strlen(message)+1, MPI_CHAR, (rank+1)%size, 0,MPI_COMM_WORLD);
      MPI_Recv(message, BSIZE, MPI_CHAR, (rank-1)%size, 0, MPI_COMM_WORLD, &status);
      printf("%s\n",message);
    }else{
      MPI_Recv(msg, BSIZE, MPI_CHAR, (rank-1)%size, 0, MPI_COMM_WORLD, &status);
      printf("%s\n",msg);
      MPI_Ssend(message, strlen(message)+1, MPI_CHAR, (rank+1)%size, 0,MPI_COMM_WORLD);

    }
    MPI_Finalize();
    return 0;
}
