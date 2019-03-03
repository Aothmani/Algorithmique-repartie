#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT 0
#define TAGCALC 1
#define NB_SITE 6
#define INITIATEUR 1

void calcul_min(int rang)
{
	int i, nb_voisins, min_local, pere, buf;
	MPI_Status status;
	int recu = 0;
	
	MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
		
	int voisins[nb_voisins];
	MPI_Recv(&voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

	/* Initiateur */
	if(rang == INITIATEUR){
		pere = -1;
		for(i = 0; i < nb_voisins; i++){
			MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGCALC, MPI_COMM_WORLD);
		}
		while(recu != nb_voisins){
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGCALC, MPI_COMM_WORLD, &status);
			if(buf < min_local)
				min_local = buf;
			recu++;
		}
		printf("Le minimum decide par le processus de rang %d est %d\n", rang, min_local);

		/* Non initiateur */	
	} else {
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGCALC, MPI_COMM_WORLD, &status);
		pere = status.MPI_SOURCE;
		if(buf < min_local)
			min_local = buf;
		for(i = 0; i < nb_voisins; i++){
			if(voisins[i] != pere){
				MPI_Send(&min_local, 1, MPI_INT, voisins[i], TAGCALC, MPI_COMM_WORLD);
			}
		}
		while( recu != nb_voisins - 1 ){
			MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, TAGCALC, MPI_COMM_WORLD, &status);
			if(buf < min_local)
				min_local = buf;
			recu++;
		}
		MPI_Send(&min_local, 1, MPI_INT, pere, TAGCALC, MPI_COMM_WORLD);
	}
}

void simulateur(void) {
	int i;

	/* nb_voisins[i] est le nombre de voisins du site i */
	int nb_voisins[NB_SITE+1] = {-1, 3, 3, 2, 3, 5, 2};
	int min_local[NB_SITE+1] = {-1, 12, 11, 8, 14, 5, 17};

	/* liste des voisins */
	int voisins[NB_SITE+1][5] = {{-1, -1, -1, -1, -1},
				     {2, 5, 3, -1, -1}, {4, 1, 5, -1, -1}, 
				     {1, 5, -1, -1, -1}, {6, 2, 5, -1, -1},
				     {1, 2, 6, 4, 3}, {4, 5, -1, -1, -1}};
                               
	for(i=1; i<=NB_SITE; i++){
		MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
		MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
	}
}

/******************************************************************************/

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
