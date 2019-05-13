#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <string.h>

#define TAGINIT 0
#define TAGSEND 1
#define NB_SITE 6
#define DIAMETRE 4
#define DEG_IN_MAX 2 /* le max des degres entrants des noeuds */
#define DEG_OUT_MAX 2 /* le max des degres sortants des noeuds */


void calcul_min(int rang)
{
	int initiateur = 0, nb_voisins_in, nb_voisins_out, min_local, i, scount = 0, rcount = 0;
	int id, buf = 0, send = 0, decision = 0;
	MPI_Status status;

	
	MPI_Recv(&nb_voisins_in, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&nb_voisins_out, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	int voisins_in[nb_voisins_in], voisins_out[nb_voisins_out], voisins_rcount[nb_voisins_in];
	memset(voisins_rcount, 0, sizeof(int) * nb_voisins_in);

	MPI_Recv(&voisins_in, nb_voisins_in, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&voisins_out, nb_voisins_out, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	srand(getpid());
	initiateur = rand() % 2;
	
	if(initiateur){
		for(i = 0; i < nb_voisins_out; i++){
			MPI_Send(&min_local, 1, MPI_INT, voisins_out[i], TAGSEND, MPI_COMM_WORLD);
		}
		scount++;
		printf("p%d> INIT : envoi du %d eme message\n", rang, scount);
	}

	while(!decision){
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGSEND, MPI_COMM_WORLD, &status);
			
		id = status.MPI_SOURCE;

		if(buf < min_local)
			min_local = buf;
		
		/* Incrementation du compteur de réception du voisin qui a envoye le message */
		for (i = 0; i < nb_voisins_in; i++){
			if (voisins_in[i] == id){
				voisins_rcount[i]++;
				printf("p%d> %d eme message recu de %d\n", rang, voisins_rcount[i], id);
			}
			
		}

		/* On verifie si les conditions d'envoi d'un message sont verifiees */
		if (scount < DIAMETRE){
			send = 1;
			for (i = 0; i < nb_voisins_in; i++){
				if (voisins_rcount[i] < scount){
					send = 0;
				}
			}
			if (send){
				for (i = 0; i < nb_voisins_out; i++){
					MPI_Send(&min_local, 1, MPI_INT, voisins_out[i], TAGSEND, MPI_COMM_WORLD);
				}
				scount++;
				printf("p%d> envoi du %d eme message avec min_local = %d\n", rang, scount, min_local);
			}			
		}

		/* On verifie si les conditions de decision sont verifiees */
		decision = 1;
		//	printf("Node %d : verification de decision\n", rang);
		for (i = 0; i < nb_voisins_in; i++){
			if(voisins_rcount[i] < DIAMETRE) {
				decision = 0;
				//	printf("Node %d : verification ratee\n", rang);
				break;
			}
		}
	}
	printf("\np%d> decision prise, le minimum est %d\n\n", rang, min_local);
}

void simulateur(void)
{
	int i;

        /* nb_voisins_in[i] est le nombre de voisins entrants du site i */
        /* nb_voisins_out[i] est le nombre de voisins sortants du site i */
	int nb_voisins_in[NB_SITE+1] = {-1, 2, 1, 1, 2, 1, 1};
	int nb_voisins_out[NB_SITE+1] = {-1, 2, 1, 1, 1, 2, 1};
	int min_local[NB_SITE+1] = {-1, 4, 7, 1, 6, 2, 9};
        /* liste des voisins entrants */
	int voisins_in[NB_SITE+1][2] = {{-1, -1},
					{4, 5}, {1, -1}, {1, -1},
					{3, 5}, {6, -1}, {2, -1}};
        /* liste des voisins sortants */
	int voisins_out[NB_SITE+1][2] = {{-1, -1},
					 {2, 3}, {6, -1}, {4, -1},
					 {1, -1}, {1, 4}, {5,-1}};
	for(i=1; i<=NB_SITE; i++){
		MPI_Send(&nb_voisins_in[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
		MPI_Send(&nb_voisins_out[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
		MPI_Send(voisins_in[i], nb_voisins_in[i], MPI_INT, i, TAGINIT,
			 MPI_COMM_WORLD);
		MPI_Send(voisins_out[i], nb_voisins_out[i], MPI_INT, i, TAGINIT,MPI_COMM_WORLD);
		MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
	}
}

int main (int argc, char* argv[]) {
	int nb_proc,rang;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

	if (nb_proc != NB_SITE+1) {
		printf("Nombre de processus incorrect !\n");
		MPI_Finalize();
		exit(2);
	}
  
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  
	if (rang == 0) {
		simulateur();
	} else {
		calcul_min(rang);
	}
  
	MPI_Finalize();
	return 0;
}
