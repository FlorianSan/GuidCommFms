#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ivy.h>
#include <ivyloop.h>
#include <errno.h>  
#include <pthread.h>
#include <string.h>

//MACRO
#define min(a,b) (a<=b?a:b) 
#define sgn(x) ((x>0)-(x<0))

pthread_mutex_t lock_roll_cmd;
pthread_mutex_t lock_gs;

//Variable globale 
float roll_cmd;
float gs; //ground spedd
int active = 1;
/* fonction associe a l'arrivée de la vitesse sol */
void groundSpeed(IvyClientPtr app, void *data, int argc, char **argv){
	pthread_mutex_lock(&lock_gs);
	gs = atof(argv[0]);
	pthread_mutex_unlock(&lock_gs);
}

/* fonction associe a l'arrivée d'information */
void calculRoulis(IvyClientPtr app, void *data, int argc, char **argv){
	
	float time = atof(argv[0]);
	float xtk = atof(argv[1]);
	float tae = atof(argv[2]);
	float dist = atof(argv[3]);
	float back_angle_ref = atof(argv[4]);

	
	float roll_commande;
	
	const float k1 = 1;
	const float k2 = 1;
	

	//TO DO fonction de check time/donnee
	
	
	pthread_mutex_lock(&lock_gs); // protection de la variable globale ground speed
	float cmd = min(bar + k1 * xtk + k2 * tae/gs, sgn(bar)*25); //Calcul de la commande de roulis
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
	roll_cmd = cmd;
	pthread_mutex_unlock(&lock_roll_cmd);
	
}
/* fonction associe a l'horloge */
void envoi(IvyClientPtr app, void *data, int argc, char **argv){
	char retour[100] = "GC_CMD_ROLL =";
	/*
	if(calcul < 100){
		if(clocl%100=90){ //envoi tout les 100ms à 90ms
			IvySendMsg ("%f", roll_cmd);
			calcul++;
		}
	}
	else{active = False;}
	*/
}
/*
on verifie si le calculRoulis a eu lieu
si oui
on check si on doit envoyer notre info (100ms)
si non 
on envoi la commande précédente
si 1s 
on desarme le PA

IMPORTANT définir avec le groupe seq la periode de l'horloge 10 ou 20 ms
*/

/* fonction associe a  */
void stop (IvyClientPtr app, void *data, int argc, char **argv){
	IvyStop ();
}

int main (int argc, char**argv){

	/* handling of -b option */
	const char* bus = 0;
	if( argc == 3){
		
		if(strcmp(argv[1], "-b") == 0){
			bus = argv[2];
		}
		else{
			printf("Definir un bus -b 127.127.127.127:2010\n");
			exit(1);
		}
	}
	else{
		bus = NULL;
	}

	/* initialisation */
	IvyInit ("Guid_COMM_APP", "Bonjour de Guid COMM", 0, 0, 0, 0);
	IvyStart (bus);
	/* abonnement */
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (groundSpeed, 0, "^GT_PARAM_GS=(.*)");
	/* abonnement  */
	IvyBindMsg (calculRoulis, 0, "GS_Data Time=(.*) XTK=(.*) TAE=(.*) Dist_to_WPT=(.*) BANK_ANGLE_REF=(.*)"); //GS_Data Time="time" XTK=" " TAE=" " Dist_to_WPT=" " BANK_ANGLE_REF= " "
	//GS_Data Time=1 XTK=2 TAE=3 Dist_to_WPT=4 BANK_ANGLE_REF=5
	
	/* abonnement */
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (envoi, 0, "^Time=(.*)");
	
	/* abonnement */
	IvyBindMsg (stop, 0, "^Stop$");
	/* main loop */
	IvyMainLoop();
	return 0;
}
