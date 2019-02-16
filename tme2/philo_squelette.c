#include <stdio.h>
#include <mpi.h>

//************   LES TAGS
#define WANNA_CHOPSTICK 0		// Demande de baguette
#define CHOPSTICK_YOURS 1		// Cession de baguette
#define DONE_EATING     2		// Annonce de terminaison

//************   LES ETATS D'UN NOEUD
#define THINKING 0   // N'a pas consomme tous ses repas & n'attend pas de baguette
#define HUNGRY   1   // En attente de baguette
#define DONE     2   // A consomme tous ses repas

//************   LES REPAS
#define NB_MEALS 3	// nb total de repas a consommer par noeud

//************   LES VARIABLES MPI
int NB;               // nb total de processus
int rank;             // mon identifiant
int left, right;      // les identifiants de mes voisins gauche et droit

//************   LA TEMPORISATION
int local_clock = 0;                    // horloge locale
int clock_val;                          // valeur d'horloge recue
int meal_times[NB_MEALS];        // dates de mes repas

//************   LES ETATS LOCAUX
int local_state = THINKING;		// moi
int left_state  = THINKING;		// mon voisin de gauche
int right_state = THINKING;		// mon voisin de droite

//************   LES BAGUETTES 
int left_chopstick = 0;		// je n'ai pas celle de gauche
int right_chopstick = 0;	// je n'ai pas celle de droite

//************   LES REPAS 
int meals_eaten = 0;		// nb de repas consommes localement


//************   LES FONCTIONS   *************************** 
int max(int a, int b) {
   return (a>b?a:b);
}

void receive_message(MPI_Status *status) {

	// A completer
}

void send_message(int dest, int tag) {
	 // A completer
}

/* renvoie 0 si le noeud local et ses 2 voisins sont termines */
int check_termination() {
   // A completer
}

//************   LE PROGRAMME   *************************** 
int main(int argc, char* argv[]) {

   MPI_Status status;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &NB);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
   left = (rank + NB - 1) % NB;
   right = (rank + 1) % NB;

   while(check_termination()) {

      /* Tant qu'on n'a pas fini tous ses repas, redemander les 2 baguettes  
         a chaque fin de repas */
      if ((meals_eaten < NB_MEALS) && (local_state == THINKING)) {
         
         //demande de baguette aux 2 voisins
         //printf("P%d> Asking for chopsticks at %d\n", rank, local_clock);
	 //
	 //A Completer
         
         
      }
		
      receive_message(&status);
		
      if (status.MPI_TAG == DONE_EATING) {
         //Enregistrer qu'un voisin a fini de manger
  		
	      // A completer
	     
      } else 
         if (status.MPI_TAG == CHOPSTICK_YOURS) {
		
            //Recuperation d'une baguette
	    //
	    // 	 A completer
	    //
                           
		
         } else {

		/* c'est une demande */


		 // A Completer

          }
   }
	
   MPI_Finalize();
   return 0;
}
