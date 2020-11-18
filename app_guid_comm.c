#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ivy.h>
#include <ivyloop.h>
#include <errno.h>  
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "pid.h"

//MACRO
#define min(a,b) (a<=b?a:b) 
#define sgn(x) ((x>0)-(x<0))



pthread_mutex_t lock_roll_cmd;
pthread_mutex_t lock_bank_angle_obj;
pthread_mutex_t lock_bank_angle_aircraft;
pthread_mutex_t lock_gs;
pthread_mutex_t lock_heading_aircraft;
pthread_mutex_t lock_heading_objective;

//Variables globales protégées des accès concurents
float roll_cmd; // commande de roulis
float global_bank_angle_obj;
float gs; //ground speed
float bank_angle_aircraft;
long int heading_aircraft;
long int heading_objective;

//Variables globales 
//TODO passer les données dans un pointeur data
int active = 1; //PA est en mode actif
float _previousTime; //stockage du temps précédent pour calculRoulis (check chronologie des données
float cmd; //stockage de la commande précédente en cas de bloquage des calculs roulis
int nb_envoi = 0;



/* fonction associe a l'arrivée de la vitesse sol */
void getposition(IvyClientPtr app, void *data, int argc, char **argv){
	pthread_mutex_lock(&lock_gs);
	gs = atof(argv[8]); //on recupere la ground speed du modele avion
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_heading_aircraft);
	heading_aircraft = atof(argv[6]); //on recupere le cap du modele avion
	pthread_mutex_unlock(&lock_heading_aircraft);
}

/* fonction associe a l'arrivée d'information */
void calculBankAngleObjNav(IvyClientPtr app, void *data, int argc, char **argv){

	clock_t begin = clock();
	
	float time = atof(argv[0]);
	float xtk = atof(argv[1]);
	float tae = atof(argv[2]);
	float dist = atof(argv[3]);
	float back_angle_ref = atof(argv[4]);

	
	const float k1 = 1;
	const float k2 = 1;
	

	//TODO fonction de check donnee
	if(time < _previousTime){ //TODO temps prédictible
		fprintf(stderr,"Probleme de chronologie des données\n");
	}
	
	pthread_mutex_lock(&lock_gs); // protection de la variable globale ground speed
	float bank_angle_obj = min(back_angle_ref + k1 * xtk + k2 * tae/gs, sgn(back_angle_ref)*25); //Calcul de la commande de roulis
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_bank_angle_obj); // protection de la variable globale bank_angle
	global_bank_angle_obj = bank_angle_obj;
	pthread_mutex_unlock(&lock_bank_angle_obj);
	
	_previousTime = time;
	clock_t end = clock();
    	unsigned long micro = (end -  begin) * 1000000 / CLOCKS_PER_SEC;
    	fprintf(stderr,"Temps d'execution : %ld micro seconde\n",micro);
}

void calculBankAngleObjHead(){
	fprintf(stderr,"Ne fait rien");
	
}
void getstate(IvyClientPtr app, void *data, int argc, char **argv){
	pthread_mutex_lock(&lock_bank_angle_aircraft);
	bank_angle_aircraft = atof(argv[6]); //correspond à phi donc le bank angle mesured
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
}
void computeRollCmd(IvyClientPtr app, void *data, int argc, char **argv){

	const float kp = 1;
	const float kd = 0;
	const float ki = 0;
	
	pthread_mutex_lock(&lock_bank_angle_obj); // protection de la variable globale bank_angle
	pthread_mutex_lock(&lock_bank_angle_aircraft);
	float calcul = pid(global_bank_angle_obj, bank_angle_aircraft); //TODO
	
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
	pthread_mutex_unlock(&lock_bank_angle_obj);
	
	
	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
	roll_cmd = calcul;
	pthread_mutex_unlock(&lock_roll_cmd);
}



void Mode(IvyClientPtr app, void *data, int argc, char **argv){
	char * endPtr;
	char mode_managed[] = "Managed", mode_selected[] = "SelectedHeading";

	if(strcmp(argv[0],mode_managed) == 0){
	active = 1;
	}
	else if (strcmp(argv[0],mode_selected) == 0){
	active = 0;
	pthread_mutex_lock(&lock_heading_objective);
	heading_objective = strtol(argv[1], &endPtr, 10 ); 
	pthread_mutex_unlock(&lock_heading_objective);
	}
	else {
	active = 0;
	}
}

/* fonction associe a l'horloge */
void envoi(IvyClientPtr app, void *data, int argc, char **argv){
    int time;
    char tm[50], r_cmd[50];
    const char* arg = (argc < 1) ? "" : argv[0]; //récupère le temps
    sscanf(arg, "%d", &time);
	if(nb_envoi < 100){ //la même commande n'a pas été envoyée pendant 1 seconde
		if(time%100==90){ //envoi tout les 100ms à 90ms
		    if(pthread_mutex_trylock(&lock_roll_cmd)==0){ //si la commande est accèssible
		        cmd = roll_cmd;
	            	pthread_mutex_unlock(&lock_roll_cmd);
	            	nb_envoi = 0;
	        }
	        else {nb_envoi++;} //sinon on reprend la commande précédente déjà enregistrée dans cmd
		    char retour[100] = "GC_CMD_ROLL =";
		    sprintf(tm, "%d", time);
		    sprintf(r_cmd, "%f", cmd);
		    strcat(retour, tm); //actual time
		    strcat(retour, r_cmd); //commande, ancienne ou pas
			IvySendMsg ("%s", retour);
		}
	}
	else{active = 0;} //on désactive le PA après 1 seconde
	
}
/*
on verifie si le calculRoulis a eu lieu
si oui
on check si on doit envoyer notre info (100ms)
si non 
on envoi la commande précédente
si 1s 
on desarme le PA

appele calculRoulisHead 
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
	

	//on s'abonne à
	IvyBindMsg (getposition, 0, "^AircraftSetPosition X=(.*) Y=(.*) Altitude-ft=(.*) Roll=(.*) Pitch=(.*) Yaw=(.*) Heading=(.*) Airspeed=(.*) Groundspeed=(.*)");
	//AircraftSetPosition X=-2.0366696227720553e-16 Y=0.8693304535637149 Altitude-ft=0.0 Roll=0.0 Pitch=0.0 Yaw=0.0 Heading=360.0 Airspeed=136.06911447084232 Groundspeed=136.06911447084232
	
	/* abonnement  */
	IvyBindMsg (calculBankAngleObjNav, 0, "GS_Data Time=(.*) XTK=(.*) TAE=(.*) Dist_to_WPT=(.*) BANK_ANGLE_REF=(.*)"); //GS_Data Time="time" XTK=" " TAE=" " Dist_to_WPT=" " BANK_ANGLE_REF= " "
	//GS_Data Time=1 XTK=2 TAE=3 Dist_to_WPT=4 BANK_ANGLE_REF=5
	
	IvyBindMsg (getstate, 0, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)");//StateVector x=1610.0 y=-3.7719121413738466e-13 z=0.0 Vp=70.0 fpa=0.0 psi=6.283185307179586 phi=0.0
	
	//on s'abonne 
	IvyBindMsg (Mode, 0, "^FCULateral Mode=(.*) Val=(.*)");//FCULateral Mode=SelectedHeading Val=10"
	
	
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (envoi, 0, "^Time=(.*)");
	
	/* abonnement */
	IvyBindMsg (stop, 0, "^Stop$");
	
	/* main loop */
	IvyMainLoop();
	return 0;
}
