#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT    0
#define NB_SITE 6

void calcul_min(rang){
  int i, nb_voisins, min_local, status;
  int *voisins;
  int stop = 0;
    
  MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  MPI_Recv(&voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);

  int voisins_recus[nb_voisins-1];
  
  if(nb_voisins == 1){
    MPI_Send(&min_local, 1, MPI_INT, voisins[0], TAGCALC, MPI_COMM_WORLD);
  } else {
    int tmp;
    for(i=0; i<nb_voisins-1; i++){
      MPI_Recv(tmp, 1, MPI_INT, MPI_ANY_SOURCE, TAGCALC, MPI_COMM_WORLD, &status);
      voisins_recus[i] = status.MPI_SOURCE;
      if(tmp < min_local)
	min_local = tmp;
    }

    i=0;
    while(!stop || i<nb_voisins-1){
      /* TODO !!!!!!!!!!!!! */
      if(
    }
    
    MPI_Send(&min_local, 1, MPI_INT, , TAGCALC, MPI_COMM_WORLD);
  }
}


void simulateur(void) {
   int i;

   /* nb_voisins[i] est le nombre de voisins du site i */
   int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
   int min_local[NB_SITE+1] = {-1, 3, 11, 8, 14, 5, 17};
   
   /* liste des voisins */
   int voisins[NB_SITE+1][3] = {{-1, -1, -1},
         {2, 3, -1}, {1, 4, 5}, 
         {1, 6, -1}, {2, -1, -1},
         {2, -1, -1}, {3, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i],nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
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
