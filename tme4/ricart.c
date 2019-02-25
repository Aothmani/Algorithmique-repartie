#include <mpi.h>

/* mesage type */
#define REQ 1
#define REPLY 2

/* state */
#define ASKING 1
#define IDLE 2
#define CS 3

#define max(a,b) (a>=b?a:b)

/* var */
int nb_reply = 0;
int date_req = 0;
int h = 0;
int state = IDLE;

int rank;

int* receive()
{
  int msg, status;
  MPI_Recv(&msg, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  h = max(msg[1])+1;
  if(msg[0] == REQ){
    if(state == IDLE || (state == ASKING && 
  }
}

void request_CS()
{
  h++;
  int date_req = h;
  int msg[] = {REQ, h, rank};
  state = ASKING;
  nb_reply = 0;
  MPI_Bcast(&msg, 3, MPI_INT, rank, MPI_COMM_WORLD);

  while(state != CS){
    receive
  }
