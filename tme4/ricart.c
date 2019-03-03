#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

/* max CS request */
#define MAX_CS 3

/* mesage type */
#define REQ 1
#define REPLY 2
#define END 3

/* state */
#define ASKING 1
#define IDLE 2
#define inCS 3

#define max(a,b) (a>=b?a:b)

/* var */
int date_req = 0;
int h = 0;
int state = IDLE;
int nb_end = 0;

int rank, nb_proc, nb_reply;

struct pending_req {
	int rank;
	struct pending_req* next;	
};

int* receive(struct pending_req** pending_req)
{
	int msg[3];
	MPI_Status status;
	MPI_Recv(&msg, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	h = max(msg[1], h)+1;
	if(msg[0] == REQ){ /* Type = REQ */
		/* si state = IDLE OU state = ASKING ET (h superieur OU (h identique ET rank superieur)) */
		if(state == IDLE || (state == ASKING && (msg[1] < date_req || msg[2] < rank))){
			h++;
			msg[0] = REPLY;
			msg[1] = h;
			msg[2] = rank;
			printf("Process %d : sending authorization to %d\n", rank, status.MPI_SOURCE);
			MPI_Send(&msg, 3, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
		} else {
			/* Pending requests list is empty */
			if(*pending_req == NULL){
				*pending_req = (struct pending_req*)malloc(sizeof(struct pending_req));
				(*pending_req)->rank = status.MPI_SOURCE;
				(*pending_req)->next = NULL;
			} else {
				/* Add req at the list's tail */
				struct pending_req* ptr = *pending_req;
				while(ptr->next != NULL){
					ptr = ptr->next;
				}
				ptr->next = (struct pending_req*)malloc(sizeof(struct pending_req));
				ptr->next->rank = status.MPI_SOURCE;
				ptr->next->next = NULL;
			}
		}
	} else if(msg[0] == REPLY){ /* Type = REPLY */
		nb_reply++;
		if(nb_reply == nb_proc-1){
			state = inCS;
			printf("Process %d : entering CS\n", rank);
		}
	} else { /* Type = END */
		nb_end++;
	}
}

void request_CS(struct pending_req** pending_req)
{
	printf("Process %d : requesting CS\n", rank);
	h++;
	int i;
	date_req = h;
	int msg[3] = {REQ, h, rank};
	state = ASKING;
	nb_reply = 0;
//	MPI_Bcast(&msg, 3, MPI_INT, rank, MPI_COMM_WORLD);
	for(i = 0; i < nb_proc; i++){
		if(i != rank){
			MPI_Send(&msg, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}

	while(state != inCS){
		receive(pending_req);
	}
}

void release_CS(struct pending_req** pending_req)
{
	printf("Process %d : leaving CS\n", rank);
	h++;
	int msg[3] = {REPLY, h, rank};
	int dest = -1;
	struct pending_req* del = NULL;
	while(*pending_req != NULL){
		dest = (*pending_req)->rank;
		MPI_Send(&msg, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
		del = *pending_req;
		*pending_req = (*pending_req)->next;
		free(del);
	}
	state = IDLE;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	struct pending_req* pending_req = NULL;
	int i;

	for(i = 1; i <= MAX_CS; i++){
		request_CS(&pending_req);
		printf("Process %d : in CS%d : bla bla bla bla\n", rank, i);
		release_CS(&pending_req);
	}
	printf("\n\nProcess %d : end of all CS\n\n\n", rank);
	h++;
	int msg[3] = {END, h, rank};
	for(i = 0; i < nb_proc; i++){
		if(i != rank){
			MPI_Send(&msg, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	while(nb_end != nb_proc-1){
		receive(&pending_req);
	}
	MPI_Finalize();
	return 0;
}
