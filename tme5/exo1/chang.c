#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

#define NB_SITE 8

/* voisins */
#define VG voisins[0]
#define VD voisins[1]

/* etats */
#define ELU 0
#define INITIATEUR 1
#define BATTU 2

/* tags */
#define TAGINIT 0
#define TAGASK 1
#define TAGELU 2

void election(int rank)
{
	int state = BATTU;
	int leader = -1;
	int initiateur;
	int voisins[2];
	MPI_Status status;

	MPI_Recv(&voisins, 2, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	
	srand(getpid());
	if(rand()%2){
		state = INITIATEUR;
		printf("Proc %d INITIATEUR\n", rank);
		MPI_Send(&rank, 1, MPI_INT, VD, TAGASK, MPI_COMM_WORLD);
	}

	while(leader == -1){
		MPI_Recv(&initiateur, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if(initiateur == rank){ /* si l'initiateur du message est moi meme */
			if(status.MPI_TAG == TAGASK){ /* si c'est un message de demande, je suis elu */
				state = ELU;
				printf("\nProc %d : C'EST NOTRE PROJET !!!!!\n\n", rank);
				MPI_Send(&rank, 1, MPI_INT, VD, TAGELU, MPI_COMM_WORLD);
			} else { /* sinon il s'agit de mon message d'annonce, je peux m'arreter */ 
				leader = rank;
			}
		} else {
			if(status.MPI_TAG == TAGELU) { /* si message d'annonce, on transmet */
				leader = initiateur;
				state = BATTU;
				printf("Proc %d : nouveau president elu : processus %d\n", rank, leader);
				MPI_Send(&leader, 1, MPI_INT, VD, TAGELU, MPI_COMM_WORLD);
			} else if(state == BATTU) { /*sinon message de demande, si non initiateur on transmet */
				MPI_Send(&initiateur, 1, MPI_INT, VD, TAGASK, MPI_COMM_WORLD);
			} else { /* sinon on est initiateur */
				if(initiateur > rank){ /* si initiateur recu est plus grand, je ne suis plus initiateur */
					state = BATTU;
					MPI_Send(&initiateur, 1, MPI_INT, VD, TAGASK, MPI_COMM_WORLD);
				} else { /* on detruit le jeton */
					printf("Proc %d : jeton de %d detruit\n", rank, initiateur);
				}
			}
		}
	}
}
	
	void simulateur(void)
	{
		int i;

		/* liste des voisins */
		int voisins[NB_SITE+1][2] = {{-1, -1},
					     {6, 5}, {7, 6}, 
					     {8, 7}, {5, 8},
					     {1, 4}, {2, 1}, {3, 2}, {4, 3}};
                               
		for(i=1; i<=NB_SITE; i++){
			MPI_Send(voisins[i], 2, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
		}
	}

	int main(int argc, char* argv[])
	{
	
		int nb_proc, rank;
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

		if (nb_proc != NB_SITE+1) {
			printf("Nombre de processus incorrect !\n");
			MPI_Finalize();
			exit(2);
		}
		
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
		if (rank == 0) {
			simulateur();
		} else {
			election(rank);
		}
	
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
		MPI_Finalize();
	}
