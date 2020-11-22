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


struct varGlobFloat
{
	float value;
	int modif; //1 si l'info est produite 0 si l'info est consommé
	
};

struct varGlobLint
{
	long int value;
	int modif; //1 si l'info est produite 0 si l'info est consommé
	
};
//Variables globales protégées des accès concurents
struct varGlobFloat roll_cmd; // commande de roulis
struct varGlobFloat global_bank_angle_obj;
struct varGlobFloat gs; //ground speed
struct varGlobFloat bank_angle_aircraft;

struct varGlobLint heading_aircraft;
struct varGlobLint heading_objective;
int active = 1; //PA est en mode actif 

//Variables globales 
//TODO passer les données dans un pointeur data *(int*)data
float _previousTime; //stockage du temps précédent pour calculBankAngleObjNav (check chronologie des données
float cmd; //stockage de la commande précédente en cas de bloquage des calculs roulis
int nb_envoi = 0;
int in_test = 0; //variable globale du mode test

void erreur(char* info){
	fprintf(stderr,"Probleme %s\n",info); //voir si signal peut faire le job
}

/* fonction associe a l'arrivée de la vitesse sol */
void getposition(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1)
		printf("Entree dans getposition\n");
	/////////
	
	pthread_mutex_lock(&lock_gs);
	gs.value = atof(argv[8]); //on recupere la ground speed du modele avion
	gs.modif = 1;
	/* Test */
	if (in_test == 1)
		printf("getposition : reception gs = %f\n",gs.value);
	/////////
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_heading_aircraft);
	heading_aircraft.value = atof(argv[6]); //on recupere le cap du modele avion
	heading_aircraft.modif = 1;
	/* Test */
	if (in_test == 1)
		printf("getposition : recepetion heading = %ld\n",heading_aircraft.value);
	/////////
	pthread_mutex_unlock(&lock_heading_aircraft);
	
}

/* fonction associe a l'arrivée d'information */
void calculBankAngleObjNav(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1)
		printf("Entree dans calculBankAngleObjNav\n");
	/////////

	clock_t begin = clock();
	//TODO fonction de check donnee
	float time = atof(argv[0]);
	float xtk = atof(argv[1]);
	float tae = atof(argv[2]);
	float dist = atof(argv[3]);
	float back_angle_ref = atof(argv[4]);
	
	/* Test */
    	if (in_test == 1)
    		printf("calculBankAngleNav : reception xtk = %f | tae = %f | dist = %f | back_angle_ref = %f\n", xtk, tae, dist, back_angle_ref);
    	/////////
	
	if(time < _previousTime){ 
		fprintf(stderr,"Probleme de chronologie des données\n");
	}
	
	const float k1 = 1;
	const float k2 = 1;
	
	float bank_angle_obj;
	

	
	
	
	pthread_mutex_lock(&lock_gs); // protection de la variable globale ground speed
	if (gs.modif){
	bank_angle_obj = min(back_angle_ref + k1 * xtk + k2 * tae/gs.value, sgn(back_angle_ref)*25); //Calcul de la commande de roulis
	gs.modif = 0;
	/* Test */
	if (in_test == 1)
		printf("calculBankAngleObjNav : calcul bank_angle_obj = %f\n", bank_angle_obj);
	/////////
	}
	else{
		erreur("calculBankAngleObjNav/gs");
		bank_angle_obj = 0;
	}
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_bank_angle_obj); // protection de la variable globale bank_angle
	global_bank_angle_obj.value = bank_angle_obj;
	global_bank_angle_obj.modif =1;
	/* Test */
	if (in_test == 1)
		printf("calculBankAngleObjNav : mise a jour global_bank_angle_obj = %f\n", global_bank_angle_obj.value);
	/////////
	pthread_mutex_unlock(&lock_bank_angle_obj);
	
	/* Test */
	if (in_test == 1){
		_previousTime = time;
		clock_t end = clock();
    		unsigned long micro = (end -  begin) * 1000000 / CLOCKS_PER_SEC;
    		printf("Temps d'execution : %ld micro secondes\n",micro);
    	}
    	/////////
}

void calculBankAngleObjHead(){
	fprintf(stderr,"Ne fait rien");
	
}
void getstate(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1)
		printf("Entree dans getstate\n");
	/////////
	pthread_mutex_lock(&lock_bank_angle_aircraft);
	bank_angle_aircraft.value = atof(argv[6]); //correspond à phi donc le bank angle mesured
	bank_angle_aircraft.modif = 1;
	/* Test */
	if (in_test == 1)
		printf("getstate : recepetion bank_angle_aircraft = %f\n", bank_angle_aircraft.value);
	/////////
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
}

void computeRollCmd(){
	/* Test */
	if (in_test == 1)
		printf("Entree dans computeRollCmd\n");
	/////////

	float local_bank_angle_aircraft, local_bank_angle_obj;
	
	pthread_mutex_lock(&lock_bank_angle_aircraft); // protection de la variable bank_angle_aircraft
	if(bank_angle_aircraft.modif){
		local_bank_angle_aircraft = bank_angle_aircraft.value;
		bank_angle_aircraft.modif = 0;
		/* Test */
		if (in_test == 1)
			printf("computeRollCmd : mise a jour local_bank_angle_aircraft = %f\n", local_bank_angle_aircraft);
		/////////
	}
	else{
		erreur("computeRollCmd/bank_angle_aircraft");
	}
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
	
	pthread_mutex_lock(&lock_bank_angle_obj); // protection de la variable globale bank_angle
	if(global_bank_angle_obj.modif){
		local_bank_angle_obj = global_bank_angle_obj.value;
		global_bank_angle_obj.modif = 0;
		/* Test */
		if (in_test == 1)
			printf("computeRollCmd : mise a jour local_bank_angle_obj = %f\n", local_bank_angle_obj);
		/////////
	}
	else{
		erreur("computeRollCmd/global_bank_angle_obj");
	}
		pthread_mutex_unlock(&lock_bank_angle_obj);
	
	float calcul = pid(local_bank_angle_obj, local_bank_angle_aircraft); //calcul du pid coeff défini dans pid.c
	
	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
	roll_cmd.value = calcul;
	roll_cmd.modif = 1;
	/* Test */
	if (in_test == 1)
		printf("computeRollCmd : calcul roll_cmd = %f\n", roll_cmd.value);
	/////////
	pthread_mutex_unlock(&lock_roll_cmd);
}



void Mode(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1)
		printf("Entree dans Mode\n");
	/////////

	char * endPtr;
	char mode_managed[] = "Managed", mode_selected[] = "SelectedHeading";

	if(strcmp(argv[0],mode_managed) == 0){
	active = 1;
	/* Test */
	if (in_test == 1)
		printf("Mode : AP actif\n");
	/////////
	}
	else if (strcmp(argv[0],mode_selected) == 0){
	active = 0;
	/* Test */
	if (in_test == 1)
		printf("Mode : AP inactif\n");
	/////////
	pthread_mutex_lock(&lock_heading_objective);
	heading_objective.value = strtol(argv[1], &endPtr, 10 ); 
	heading_objective.modif = 1;
	/* Test */
	if (in_test == 1)
		printf("Mode : calcul heading_objective = %ld\n", heading_objective.value);
	/////////
	pthread_mutex_unlock(&lock_heading_objective);
	}
	else {
	active = 0;
	/* Test */
	if (in_test == 1)
		printf("Mode : AP inactif\n");
	/////////
	}
}

/* fonction associe a l'horloge */
void envoi(IvyClientPtr app, void *data, int argc, char **argv){
    /* Test */
    if (in_test == 1)
	printf("Entree dans envoi\n");
    /////////

    char tm[50], r_cmd[50];
    int time = atof(argv[0]) * 1000;
    
    ////////////////////////////
    /*
    computeRollCmd();
    cmd = roll_cmd.value;
    char retour[100] = "GC_CMD_ROLL =";
    sprintf(tm, "%d", time);
    sprintf(r_cmd, "%f", cmd);
    strcat(retour, tm); //actual time
    strcat(retour, r_cmd); //commande, ancienne ou pas
    IvySendMsg ("%s", retour);
    */
    ///////////////////////////
    
    
    if(time%100==70){ //On calcule la commande durée à définir
        computeRollCmd(); 
        fprintf(stderr,"out calcul roll cmd\n");  
    }
    else if(time%100==90){ //envoi tout les 100ms à 90ms
	    if(nb_envoi < 100 && active){ //la même commande ne doit pas être envoyée pendant plus d'une seconde et le PA doit être actif
		    if(pthread_mutex_trylock(&lock_roll_cmd)==0){ //si la commande est accèssible
		    	if(roll_cmd.modif){
		        	cmd = roll_cmd.value;
		        	roll_cmd.modif = 0;
		        }
		        else{
		        	erreur("envoi/roll_cmd");
		        }
	            	pthread_mutex_unlock(&lock_roll_cmd);
	            	nb_envoi = 0;
			}
		    else{  //sinon on reprend la commande précédente déjà enregistrée dans cmd
			    nb_envoi++;
		    } 
		    char retour[100] = "GC_CMD_ROLL =";
		    sprintf(tm, "%d", time);
		    sprintf(r_cmd, "%f", cmd);
		    strcat(retour, tm); //actual time
		    strcat(retour, r_cmd); //commande, ancienne ou pas
		    IvySendMsg ("%s", retour);
		    fprintf(stderr,"send");
		}
		else{active = 0;} //on désactive le PA après 1 seconde
	}
}



/* fonction associe a  */
void stop(IvyClientPtr app, void *data, int argc, char **argv){
	IvyStop ();
}

int main (int argc, char**argv){

	const char* bus = 0;
	
	/* handling of only -t option */
	if( argc == 2){
		if(strcmp(argv[1], "-t") == 0){
			in_test = 1;
			printf("App ready to be tested\n");
		}
		else{
			printf("Argument invalide\n");
			exit(1);
		}
	}
	
	/* handling of only -b option */
	else if( argc == 3){
		
		if(strcmp(argv[1], "-b") == 0){
			bus = argv[2];
		}
		else{
			printf("Arguments invalides. Definir un bus -b 127.127.127.127:2010\n");
			exit(1);
		}
	}
	
	/*handling of -b en -t options */
	else if( argc == 4){
		if((strcmp(argv[1], "-b") == 0) && (strcmp(argv[3], "-t") == 0)){
			bus = argv[2];
			in_test = 1;
			printf("App ready to be tested\n");
		}
		else if ((strcmp(argv[1], "-t") == 0) && (strcmp(argv[2], "-b") == 0)){
			bus = argv[3];
			in_test = 1;
			printf("App ready to be tested\n");
		}
		else{
			printf("Arguments invalides. Definir un bus -b 127.127.127.127:2010\n");
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
	IvyBindMsg (calculBankAngleObjNav, 0, "GS_Data Time=(.*) XTK=(.*) TAE=(.*) DTWPT=(.*) BANK_ANGLE_REF=(.*)"); //GS_Data Time="time" XTK=" " TAE=" " Dist_to_WPT=" " BANK_ANGLE_REF= " "
	//GS_Data Time=1 XTK=2 TAE=3 Dist_to_WPT=4 BANK_ANGLE_REF=5
	
	IvyBindMsg (getstate, 0, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)");//StateVector x=1610.0 y=-3.7719121413738466e-13 z=0.0 Vp=70.0 fpa=0.0 psi=6.283185307179586 phi=0.0
	
	//on s'abonne 
	IvyBindMsg (Mode, 0, "^FCULateral Mode=(.*) Val=(.*)");//FCULateral Mode=SelectedHeading Val=10"
	
	
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (envoi, 0, "^Time t=(.*)");
	
	/* abonnement */
	IvyBindMsg (stop, 0, "^Stop$");
	
	/* main loop */
	IvyMainLoop();
	return 0;
}
