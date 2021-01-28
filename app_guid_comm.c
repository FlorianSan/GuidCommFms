#include "app_guid_comm.h"


//Récupère la ground speed et le cap du modèle avion
/* fonction associe a l'arrivée de la vitesse sol */
void getPosition(IvyClientPtr app, void *data, int argc, char **argv){
	debut = clock();
	/* Test */
	if (in_test == 1){
		printf("Entree dans getPosition\n");
	}
	/////////
	
	pthread_mutex_lock(&lock_gs);
	
	/* Acquisition de gs*/
	if (testFormat(argv[8], "float")){
		gs.value = atof(argv[8]);
		gs.modif = 1;
	}
	else{error("getPosition", "gs");}
		
	/* Test */
	if (in_test == 1){
		printf("getPosition : reception gs = %f\n",gs.value);
	}
	/////////
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_heading_aircraft);
	
	/* Acquisition de heading_aircraft */
	if (testFormat(argv[6], "int")){
		heading_aircraft.value = atol(argv[6]);
		heading_aircraft.modif = 1;
	}
	else{error("getPosition", "heading_aircraft");}
	
	/* Test */
	if (in_test == 1){
		printf("getPosition : recepetion heading = %ld\n",heading_aircraft.value);
	}
	/////////
	pthread_mutex_unlock(&lock_heading_aircraft);
	
}

//Récupère le bank angle mesuré du modèle avion
void getState(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1){
		printf("Entree dans getState\n");
	}
		
	/////////////////////////////////////////////////////////////////////////////////////////
	//récupération de Vp
	/////////////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_vp);
	
	if (testFormat(argv[3], "float")){
		vp.value = atof(argv[3]);
		vp.modif = 1;
	}
	else{error("getState", "vp");}
	
	/* Test */
	if (in_test == 1){
		printf("getState : recepetion Vp = %f\n", vp.value);
	}
	/////////
	pthread_mutex_unlock(&lock_vp);
	
	
	/////////////////////////////////////////////////////////////////////////////////////////
	//récupération du fpa
	/////////////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_fpa);
	if (testFormat(argv[4], "float")){
		fpa.value = atof(argv[4]);
		fpa.modif = 1;
	}
	else{error("getState", "fpa");}
	
	/* Test */
	if (in_test == 1){
		printf("getState : recepetion fpa = %f\n", fpa.value);
	}
	/////////
	pthread_mutex_unlock(&lock_fpa);
	
	/////////////////////////////////////////////////////////////////////////////////////////
	//récupération du bank_angle_aircraft
	/////////////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_bank_angle_aircraft);

	if (testFormat(argv[6], "float")){
		bank_angle_aircraft.value = atof(argv[6]); //correspond à phi donc le bank angle mesured
		bank_angle_aircraft.modif = 1;
	}
	else {error("getState", "bank_angle_aircraft");}
	
	/* Test */
	if (in_test == 1){
		printf("getState : recepetion bank_angle_aircraft = %f\n", bank_angle_aircraft.value);
	}
	/////////
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
}


void getForecast(IvyClientPtr app, void *data, int argc, char **argv){
	if (in_test == 1){
		printf("Entree dans getForecast\n");
	}
	
	pthread_mutex_lock(&lock_TAS);
	if (testFormat(argv[0], "float")){
		tas.value = atof(argv[0]);
		tas.modif = 1;
	}
	
	else{error("computeBankAngleObj", "tae");}
	pthread_mutex_unlock(&lock_TAS);
	
	pthread_mutex_lock(&lock_Alt);
	if (testFormat(argv[1], "float")){
		alt.value = atof(argv[1]);
		alt.modif = 1;
	}
	else{error("computeBankAngleObj", "tae");}
	pthread_mutex_unlock(&lock_Alt);
	
	if (in_test == 1){
		printf("getForecast : recepetion TAS = %f, Altitude = %f\n", tas.value, alt.value);
	}
}

//Calcule le bank angle souhaité (pour suivre ou revenir sur la trajectoire)
/* fonction associe a l'arrivée d'information */
void computeBankAngleObj(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	clock_t begin;
	if (in_test == 1){
		printf("Entree dans computeBankAngleObj\n");
		begin = clock();
	}
	/////////
	
	//Acquisition des données de GUID-SEQ
	if (testFormat(argv[1], "float")){
		xtk.value = atof(argv[1]) * 1852; 	    //xtk de seq en Nm donc conversion en metre
		xtk.modif = 1;
	}
	else{error("computeBankAngleObj", "xtk");}
	
	if (testFormat(argv[2], "float")){
		tae.value = atof(argv[2]); 	    //tae de seq en rad
		tae.modif = 1;
	}
	else{error("computeBankAngleObj", "tae");}
	
	if (testFormat(argv[3], "float")){
		dist.value = atof(argv[3]);	    //dist de seq en m
		dist.modif = 1;
	}
	else{error("computeBankAngleObj", "dist");}
	
	if (testFormat(argv[4], "float")){
		bank_angle_ref.value = atof(argv[4]); //Bank angle
		bank_angle_ref.modif = 1;
	}
	else{error("computeBankAngleObj", "bank_angle_ref");}	
		
	/* Test */
    	if (in_test == 1){
    		printf("computeBankAngleNav : reception xtk = %f | tae = %f | dist = %f | bank_angle_ref = %f\n", xtk.value, tae.value, dist.value, bank_angle_ref.value);
    	}
    	/////////
	/*Si on le fait, à faire dans la fonction test (et rajouter l'acquisition du temps au début de la fonction)
	if(time < (*(variables*)data).param){ 
		fprintf(stderr,"Probleme de chronologie des données\n");
	}*/
	
	float bank_angle_obj; 
	
	pthread_mutex_lock(&lock_gs); // protection de la variable globale ground speed
	
	
	
	//dans le mode managé
	if (gs.modif){
	
	    pthread_mutex_lock(&lock_ap_mode);
	    int local_ap_mode = ap_mode; //comme il n'y à pas de modif de ap_mode
	    pthread_mutex_unlock(&lock_ap_mode);
	
		if(local_ap_mode){
			bank_angle_obj = computeBankAngleObjNav(gs.value); //Calcul de la commande 
		}
		else if (local_ap_mode == 0){
			pthread_mutex_lock(&lock_heading_aircraft);
			pthread_mutex_lock(&lock_heading_objective);
			bank_angle_obj = computeBankAngleObjHdg();
			heading_aircraft.modif = 0;//on a utilisé la donnée
			heading_objective.modif = 0;//on a utilisé la donnée
			pthread_mutex_unlock(&lock_heading_aircraft);
			pthread_mutex_unlock(&lock_heading_objective);
		}
		gs.modif = 0;
		/* Test */
		if (in_test == 1){
			if(local_ap_mode){
				printf("computeBankAngleObjNav : calcul bank_angle_obj_nav = %f\n", bank_angle_obj);
			}
			else if (local_ap_mode == 0){
				printf("computeBankAngleObjHdg : calcul bank_angle_obj_hdg = %f\n", bank_angle_obj);
			}
		}
		/////////
	}
	else{
		bank_angle_obj = 0; //à vérifier auprès de Guy
	}
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_bank_angle_objective);
	global_bank_angle_obj.value = bank_angle_obj;
	global_bank_angle_obj.modif = 1;
	pthread_mutex_unlock(&lock_bank_angle_objective);
	
	computeCmd(bank_angle_obj);
	
	
	/* Test */
	if (in_test == 1){
		/*_previousTime = time; Cela n'a pas sa place ici ??*/
		clock_t end = clock();
    		unsigned long micro = (end -  begin) * 1000000 / CLOCKS_PER_SEC;
    		printf("Temps d'execution : %ld micro secondes\n",micro);
    	}
    	/////////
}

//Etablir si on est en mode Nav ou Hdg (resp. Selected ou Managed)
void getMode(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1){
		printf("Entree dans getMode\n");
	}
	/////////

	char mode_managed[] = "Managed", mode_selected[] = "SelectedHeading";
	
    	pthread_mutex_lock(&lock_ap_mode);
	
	if(strcmp(argv[0],mode_managed) == 0){
	    ap_mode = 1;
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : AP actif\n");
	    }
	/////////
	}
	else if (strcmp(argv[0],mode_selected) == 0){
	    ap_mode = 0;
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : AP inactif\n");
	    }
	    /////////
	    pthread_mutex_lock(&lock_heading_objective);
	    
	    /* Acquisition du heading_objective */
	    if (testFormat(argv[1], "int")){
		    heading_objective.value = atol(argv[1]);
		    heading_objective.modif = 1;
	    }
	    else{error("getMode", "heading_objective");}
	    
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : calcul heading_objective = %ld\n", heading_objective.value);
		}
	    /////////
	    pthread_mutex_unlock(&lock_heading_objective);
	}
	else {
	    ap_mode = 0;
	    
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : AP inactif\n");
		}
	    /////////
	}
	pthread_mutex_unlock(&lock_ap_mode);
}

//Envoie les commande à l'avion
/* fonction associe a l'horloge */
void sendGC(IvyClientPtr app, void *data, int argc, char **argv){
    /* Test */
    if (in_test == 1){
	    printf("Entree dans sendGC\n");
	}
    	/////////
    
	char tm[50], rollCommande[100], nxCommande[100], nzCommande[100], apState[100], bankAngle[100], trackObj[100];

	/* Acquisition du temps */
	if (testFormat(argv[0], "int")){
	    current_time.value = atof(argv[0])*1000;
	    current_time.modif = 1;
	}
	else{error("sendGC", "current_time");}
	
    if(flight_starting.value == 1){
	    pthread_mutex_lock(&lock_ap_mode);
		int local_ap_mode = ap_mode; //comme il n'y à pas de modif de ap_mode
		pthread_mutex_unlock(&lock_ap_mode);
		
		
		//TODO envoyer l'état du PA toutes les secondes d'après doc point focaux
		if(current_time.value%1000 == 0){
		    if(local_ap_mode == 0){
			sprintf(apState, "GC_AP Time=%ld AP_State='Activated' AP_Mode='NAV'", current_time.value);
		    }
		    else if(local_ap_mode == 1){
			sprintf(apState, "GC_AP Time=%ld AP_State='Activated' AP_Mode='HDG'", current_time.value);
		    }
		IvySendMsg("%s", apState);
		if (in_test == 1){
			printf("AP state : %d \n", local_ap_mode);
		    }
	    }
	    
	    /////////////////////////////////////////////////////////////////////////////////
	    //Envoi de bankAngleObj
	    /////////////////////////////////////////////////////////////////////////////////
	    pthread_mutex_lock(&lock_bank_angle_objective);
	    if(global_bank_angle_obj.modif){
	        global_bank_angle_obj.modif = 0; //on a utilisé la valeur
	    }
	    else {error("sendGC", "global_bank_angle_obj");}
		sprintf(bankAngle, "GC_BA Time=%ld Bank_Angle_Obj=%f", current_time.value, global_bank_angle_obj.value); 
		IvySendMsg ("%s", bankAngle);
		pthread_mutex_unlock(&lock_bank_angle_objective);
		
	    /////////////////////////////////////////////////////////////////////////////////
	    //Envoi de roll cmd
	    /////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_roll_cmd); //récupère la valeur de commande à voir si on refait un try
		if(roll_cmd.modif){
			(*(float*)data) = roll_cmd.value;
			roll_cmd.modif = 0;
		}
		else {error("sendGC", "roll_cmd");}
		pthread_mutex_unlock(&lock_roll_cmd);

		sprintf(rollCommande, "APLatControl rollRate=%f", (*(float*)data)); //commande, ancienne ou pas APLatControl rollRate=1
		IvySendMsg ("%s", rollCommande);

	 	/* Test */
		if (in_test == 1)
			printf("rollRate= %f time = %ld\n", (*(float*)data), current_time.value);
		/////////


		/////////////////////////////////////////////////////////////////////////////////
		//Envoi de nx cmd
		/////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_nx_cmd); // protection de la variable globale nx_cmd
		if (nx_cmd.modif){
			nx_cmd.modif = 0;
		}
		else {error("sendGC", "nx_cmd");}
		sprintf(nxCommande, "APNxControl nx=%f", nx_cmd.value); //commande, ancienne ou pas
		IvySendMsg ("%s", nxCommande);
		pthread_mutex_unlock(&lock_nx_cmd);


		/////////////////////////////////////////////////////////////////////////////////
		//Envoi de nz cmd
		/////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_nz_cmd); // protection de la variable globale nz_cmd
		if (nz_cmd.modif){
			nz_cmd.modif = 0;
		}
		else {error("sendGC", "nz_cmd");}
		sprintf(nzCommande, "APNzControl nz=%f", nz_cmd.value); //commande, ancienne ou pas
		IvySendMsg ("%s", nzCommande);
		pthread_mutex_unlock(&lock_nz_cmd);
		
		/////////////////////////////////////////////////////////////////////////////////
		//Envoi de track objective
		/////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_track_obj); // protection de la variable globale track_obj
		if (track_obj.modif){
			track_obj.modif = 0;
		}
		else {error("sendGC", "track_obj");}
		sprintf(trackObj, "GC_TO Time=%ld Track_Obj=%ld", current_time.value, track_obj.value); //commande, ancienne ou pas
		IvySendMsg ("%s", trackObj);
		pthread_mutex_unlock(&lock_track_obj);
		
		fin = clock();
		unsigned long micro_tot = (fin -  debut) * 1000000 / CLOCKS_PER_SEC;
	    	printf("Temps d'execution : %ld micro secondes\n",micro_tot);
	}
	else{
		printf("Avion pas encore en vol\n");
	}
	
}


/* fonction associe a  */
void start_flight(IvyClientPtr app, void *data, int argc, char **argv){
	flight_starting.value = 1;
}

/* fonction associe a  */
void restart(IvyClientPtr app, void *data, int argc, char **argv){
	IvyStop();
}

//Fonction de stop avec Ivy
void stop(IvyClientPtr app, void *data, int argc, char **argv){
    //On tue les affichages
	int status4 = system("pkill -f horizon_artificiel.py &");
	int status5 = system("pkill -f affichage_commande.py &");
	int status6 = system("pkill -f affichage_position.py &");
	//On tue l'app
	exit(EXIT_SUCCESS);
}
//Fonction de stop avec ctl+c
void intHandler(int dummy) {
    //On tue les affichages
	int status4 = system("pkill -f horizon_artificiel.py &");
	int status5 = system("pkill -f affichage_commande.py &");
	int status6 = system("pkill -f affichage_position.py &");
	//On tue l'app
	exit(EXIT_SUCCESS);
}

int start(const char* bus, float sendCmd){
    	/* initialisation */
	IvyInit ("GUID_COMM_APP", "Bonjour de GUID COMM", 0, 0, 0, 0);
	IvyStart (bus);
	

	//on s'abonne à
	IvyBindMsg (getPosition, 0, "^AircraftSetPosition X=(.*) Y=(.*) Altitude-ft=(.*) Roll=(.*) Pitch=(.*) Yaw=(.*) Heading=(.*) Airspeed=(.*) Groundspeed=(.*)");
	//AircraftSetPosition X=-2.0366696227720553e-16 Y=0.8693304535637149 Altitude-ft=0.0 Roll=0.0 Pitch=0.0 Yaw=0.0 Heading=360.0 Airspeed=136.06911447084232 Groundspeed=136.06911447084232
	
	IvyBindMsg (getState, 0, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)");//StateVector x=1610.0 y=-3.7719121413738466e-13 z=0.0 Vp=70.0 fpa=0.0 psi=6.283185307179586 phi=0.0
	
	IvyBindMsg (getForecast, 0, "GT TAS=(.*) CRZ_ALT=(.*)"); //"GT TAS=150.0 CRZ_ALT=1000"
	
	// abonnement  
	IvyBindMsg (computeBankAngleObj, 0, "^GS_Data Time=(.*) XTK=(.*) TAE=(.*) DTWPT=(.*) BANK_ANGLE_REF=(.*) ALDTWPT=(.*)"); 
	
	//on s'abonne 
	IvyBindMsg (getMode, 0, "^FCULateral Mode=(.*) Val=(.*)");//FCULateral Mode=SelectedHeading Val=10"
	
	
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (sendGC, &sendCmd, "^Time t=(.*)");
	
	
	//on s'abonne au demmarage du vol
	IvyBindMsg (start_flight, 0, "InitStateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)");
	
	
	// abonnement 
	IvyBindMsg (restart, 0, "^Restart_GUID_COMM$");
	
	// abonnement 
	IvyBindMsg (stop, 0, "^Stop_GUID_COMM$");
	
	signal(SIGINT, intHandler); //abonnement au raccourci ctrl+c
	
	// main loop 
	IvyMainLoop();
}

int main (int argc, char**argv){

	int nb_try = 0; //nombre de fois où on a démarré l'app (l'app ne redémarre plus si trop d'erreurs)
	const char* bus = 0;
    	int nb_sent = 0; //A voir avec la fonction sendGC
    	//Garde la dernière valeur reçue en cas de panne
	float sendCmd; 
	
	//Initialisation test
	in_test = 0;
	
	//Initialisation structure
	pthread_mutex_lock(&lock_nz_cmd); 
		nz_cmd.value = 1;
		nz_cmd. modif = 1;
	pthread_mutex_unlock(&lock_nz_cmd);
	
    pthread_mutex_lock(&lock_roll_cmd); 
		roll_cmd.value = 0;
		roll_cmd.modif = 1;
	pthread_mutex_unlock(&lock_roll_cmd);
	
	pthread_mutex_lock(&lock_nx_cmd); 
		nx_cmd.value = 0;
		nx_cmd.modif = 1;
	pthread_mutex_unlock(&lock_nx_cmd);
	
	
	flight_starting.value = 0;
	
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
	
	/////////////////////////////////////////////////////////////////////////////////
    //Lancement IHM
    /////////////////////////////////////////////////////////////////////////////////
	int status1 = system("python3 affichage/horizon_artificiel.py &");
	int status2 = system("python3 affichage/affichage_commande.py &");
	int status3 = system("python3 affichage/affichage_position.py &");
	

    /////////////////////////////////////////////////////////////////////////////////
    //Boucle principale, permet un redemarrage de l'application suite à un plantage.
    /////////////////////////////////////////////////////////////////////////////////
	//while(nb_try < 4){
	while(1){
		nb_try++;
		//ATTENTION PA actif à chaque démarragef
		pthread_mutex_lock(&lock_ap_mode);
		ap_mode = 1;
		pthread_mutex_unlock(&lock_ap_mode);
		//Si erreurs, fonction error arrête l'app
		//Remise à zéro des erreurs en cas de redémarrage à chaud
		error_init(); 
	    	//appel de la fonction principale
    		start(bus, sendCmd);
    		if (in_test == 1)
        		printf("REDEMARAGE FONCTION\n");
	}
	printf("APP CRASHED\n");
	IvySendMsg("GC_AP Time=%ld AP_State='Deactivated' AP_Mode='NULL' ", current_time.value); //on prévient TRAJ de la désactivation du PA
	return 0;
}
