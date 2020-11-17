#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ivy.h>
#include <ivyloop.h>
#include <errno.h>  
#include <pthread.h>
#include <string.h>
#include <time.h>

//MACRO
#define min(a,b) (a<=b?a:b) 
#define sgn(x) ((x>0)-(x<0))

pthread_mutex_t lock_roll_cmd;
pthread_mutex_t lock_gs;

//Variables globales protégées des accès concurents
float roll_cmd; // commande de roulis
float gs; //ground speed

//Variables globales 
//TO DO passer les données dans un pointeur data
int active = 1; //PA est en mode actif
float _previousTime; //stockage du temps précédent pour calculRoulis (check chronologie des données
float cmd; //stockage de la commande précédente en cas de bloquage des calculs roulis
int nb_envoi = 0;



/* fonction associe a l'arrivée de la vitesse sol */
void groundSpeed(IvyClientPtr app, void *data, int argc, char **argv){
	pthread_mutex_lock(&lock_gs);
	gs = atof(argv[0]);
	pthread_mutex_unlock(&lock_gs);
}

/* fonction associe a l'arrivée d'information */
void calculRoulisNav(IvyClientPtr app, void *data, int argc, char **argv){

	clock_t begin = clock();
	
	float time = atof(argv[0]);
	float xtk = atof(argv[1]);
	float tae = atof(argv[2]);
	float dist = atof(argv[3]);
	float back_angle_ref = atof(argv[4]);

	
	float roll_commande;
	
	const float k1 = 1;
	const float k2 = 1;
	

	//TO DO fonction de check time/donnee
	if(time < _previousTime){
	fprintf(stderr,"Probleme de chronologie des données\n");
	}
	
	pthread_mutex_lock(&lock_gs); // protection de la variable globale ground speed
	float cmd = min(back_angle_ref + k1 * xtk + k2 * tae/gs, sgn(back_angle_ref)*25); //Calcul de la commande de roulis
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
	roll_cmd = cmd;
	pthread_mutex_unlock(&lock_roll_cmd);
	
	_previousTime = time;
	clock_t end = clock();
    	unsigned long micro = (end -  begin) * 1000000 / CLOCKS_PER_SEC;
    	fprintf(stderr,"Temps d'execution : %ld micro seconde\n",micro);
}

void calculRoulisHead(){
	fprintf(stderr,"Ne fait rien");
	
}

void Mode(IvyClientPtr app, void *data, int argc, char **argv){
    int hdg;
    char mode_managed[] = "Managed", mode_selected[] = "SelectedHeading";
    
    if(strcmp(argv[0],mode_managed) == 0){
        active = 1;
    }
    else if (strcmp(argv[0],mode_selected) == 0){
        active = 0;
        hdg = atof(argv[1]); //à vérifier
        
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
	/* abonnement */
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (groundSpeed, 0, "^GT_PARAM_GS=(.*)");//GT_PARAM_GS=240
	/* abonnement  */
	IvyBindMsg (calculRoulisNav, 0, "GS_Data Time=(.*) XTK=(.*) TAE=(.*) Dist_to_WPT=(.*) BANK_ANGLE_REF=(.*)"); //GS_Data Time="time" XTK=" " TAE=" " Dist_to_WPT=" " BANK_ANGLE_REF= " "
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
