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
#define NB_MEALS 4	// nb total de repas a consommer par noeud

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

int receive_message(MPI_Status *status) {
  int clock;
  MPI_Recv(&clock, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
  return clock;
}

void send_message(int dest, int tag) {
  MPI_Send(&local_clock, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
}

/* renvoie 0 si le noeud local et ses 2 voisins sont termines */
int check_termination() {
  if(local_state == DONE && left_state == DONE && right_state == DONE){
    return 0;
  }
  return 1;
}

void list_meals(){
  int i;
  for(i = 0; i < NB_MEALS; i++){
    printf("P%d> Meal n°%d eaten at %d\n", rank, i, meal_times[i]);
  }
}

//************   LE PROGRAMME   *************************** 
int main(int argc, char* argv[]) {

  MPI_Status status;
  int i;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &NB);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
  left = (rank + NB - 1) % NB;
  right = (rank + 1) % NB;

  while(check_termination()) {

    /* Tant qu'on n'a pas fini tous ses repas, redemander les 2 baguettes  
       a chaque fin de repas */
    if ((meals_eaten < NB_MEALS) && (local_state == THINKING)) {
      local_state = HUNGRY;
      //demande de baguette aux 2 voisins
      send_message(left, WANNA_CHOPSTICK);
      send_message(right, WANNA_CHOPSTICK);
      printf("P%d> Asking for chopsticks at %d\n", rank, local_clock);

    }


    int clock = receive_message(&status);
    if(clock > local_clock)
      local_clock = clock;
    
    
    if (status.MPI_TAG == DONE_EATING) {
      //Enregistrer qu'un voisin a fini de manger
      if(status.MPI_SOURCE == left){
	left_state = DONE;
      } else {
	right_state = DONE;
      }
		     
    } else {
      if (status.MPI_TAG == CHOPSTICK_YOURS) {
		
	//Recuperation d'une baguette
	if(local_state == HUNGRY){
	  if(status.MPI_SOURCE == left){
	    left_chopstick = 1;
	  } else {
	    right_chopstick = 1;
	  }
	}

	if(left_chopstick == 1 && right_chopstick == 1){
	  printf("P%d> Eating at %d\n", rank, local_clock);
	  meal_times[meals_eaten] = local_clock;
	  meals_eaten++;
	  left_chopstick = 0;
	  right_chopstick = 0;
	  if( meals_eaten == NB_MEALS){
	    list_meals();
	    local_state = DONE;
	    send_message(left, DONE_EATING);
	    send_message(right, DONE_EATING);
	  } else {
	    local_state = THINKING;
	  }
	  
	  if(left_state == HUNGRY)
	    send_message(left, CHOPSTICK_YOURS);
	  if(right_state == HUNGRY)
	    send_message(right, CHOPSTICK_YOURS);
	}
	
		
      } else {

	/* c'est une demande */
	if(status.MPI_SOURCE == left){
	  left_state = HUNGRY;
	} else {
	  right_state = HUNGRY;
	}

	if(local_state != HUNGRY || rank > status.MPI_SOURCE){
	  if(status.MPI_SOURCE == left){
	    left_chopstick = 0;
	  } else {
	    right_chopstick = 0;
	  }
	  send_message(status.MPI_SOURCE, CHOPSTICK_YOURS);
	}
      }
    }

    local_clock++;
  }

  printf("P%d> Terminating\n", rank);
  MPI_Finalize();
  return 0;
}
