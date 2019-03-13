#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* etats */
#define INITIATEUR 0
#define SUIVEUR 1

#define TAGINIT 0
#define TAGREVEIL 1
#define TAGIDENT 2
#define NB_SITE 6

void calcul_min(int rang)
{

  	MPI_Status status;
	int i, j, nb_voisins;
	int  min_local = rang, found = 0, buf = 0, nb_recu = 0;
	int init = 1;
	int id_elu = -1;
	
	MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	int feuille = (nb_voisins == 1);	
	int voisins[nb_voisins];
	
	MPI_Recv(&voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	
	int voisins_recus[nb_voisins];
	memset(&voisins_recus, 0, nb_voisins);
	for(i=0; i<nb_voisins; i++){
	         voisins_recus[i] = 0;
	}

	/* envoi des messages des initiateurs */
	srand(getpid());
	if(rand()%2){ /* si initiateur */
		printf("Proc %d INITIATEUR\n", rang);
		init = 0;
		if(feuille){ /* si feuille, envoi de rank au voisin */
			MPI_Send(&min_local, 1, MPI_INT, voisins[0], TAGIDENT, MPI_COMM_WORLD);
	     	} else { /* si noeud interne, envoi de reveil a tous les voisins */
			for(i=0; i<nb_voisins; i++){ 
				MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGREVEIL, MPI_COMM_WORLD);
			}
		}	
	}


	while(id_elu == -1){
		//printf("Message recu \n");
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if(status.MPI_TAG == TAGREVEIL){ /* TAG reveil */
			if(feuille){
				if (init){
					init = 0;
					MPI_Send(&rang, 1, MPI_INT, voisins[0], TAGIDENT, MPI_COMM_WORLD);
				}
			} else {
				if (init){
					init = 0;
					printf("Proc %d : reveil des autres proc\n", rang);
					for(i=0; i<nb_voisins; i++){
						MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGREVEIL, MPI_COMM_WORLD);
					}
				}
			}
		} else { /* TAG identite */
			if(feuille){
				id_elu = (rang < buf) ? rang : buf;
				printf("Proc %d : election du nouveau president %d\n", rang, id_elu);
			} else {
				if(init){
					init = 0;
					for(i=0; i<nb_voisins; i++){ 
						MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGREVEIL, MPI_COMM_WORLD);
					}
				}
				for(i=0; i<nb_voisins; i++){ 
					if(voisins[i] == status.MPI_SOURCE){
						voisins_recus[i] = 1;
						nb_recu++;
					}
				}
				min_local = (min_local < buf) ? min_local : buf;
				if(nb_recu == nb_voisins - 1){
					for(i = 0; i < nb_voisins; i++){
						if(voisins_recus[i] == 0){
							MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGIDENT, MPI_COMM_WORLD);
						}
					}
										 
				}
				if(nb_recu == nb_voisins){
					id_elu = (min_local < buf) ? min_local : buf;
					printf("Proc %d : election du nouveau president %d\n", rang, id_elu);
					for(i = 0; i < nb_voisins; i++){
						if(voisins[i] != status.MPI_SOURCE){
							MPI_Send(&id_elu, 1, MPI_INT, voisins[i], TAGIDENT, MPI_COMM_WORLD);
						}
					}
				}
			}
		}
	}
}

void simulateur(void)
{
	int i;

	/* nb_voisins[i] est le nombre de voisins du site i */
	int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
   
	/* liste des voisins */
	int voisins[NB_SITE+1][3] = {{-1, -1, -1},
				     {2, 3, -1}, {1, 4, 5}, 
				     {1, 6, -1}, {2, -1, -1},
				     {2, -1, -1}, {3, -1, -1}};
                               
	for(i=1; i<=NB_SITE; i++){
		MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
	}
}

/******************************************************************************/

int main (int argc, char* argv[])
{
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
