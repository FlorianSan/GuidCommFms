#include "app_guid_comm.h"


//Save te ground speed and the aircraft heading
/* function associated with aircraft position information */
void getPosition(IvyClientPtr app, void *data, int argc, char **argv){
	debut = clock();
	/* Test */
	if (in_test == 1){
		printf("Entry into getPosition\n");
	}
	/////////
	
	pthread_mutex_lock(&lock_gs);
	
	/* gs acquisition */
	if (testFormat(argv[8], "float")){
		gs.value = atof(argv[8]);
		gs.modif = 1; //data updated
	}
	else{error("getPosition", "gs");}
		
	/* Test */
	if (in_test == 1){
		printf("getPosition : reception gs = %f\n",gs.value);
	}
	/////////
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_heading_aircraft);
	
	/* Heading_aircraft acquisition */
	if (testFormat(argv[6], "int")){
		heading_aircraft.value = atol(argv[6]);
		heading_aircraft.modif = 1; //data updated
	}
	else{error("getPosition", "heading_aircraft");}
	
	/* Test */
	if (in_test == 1){
		printf("getPosition : recepetion heading = %ld\n",heading_aircraft.value);
	}
	/////////
	pthread_mutex_unlock(&lock_heading_aircraft);
	
}

//Save the mesured aircraft bank angle from aircraft model
/* function associated with aircraft state information */
void getState(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1){
		printf("Entry into getState\n");
	}
		
	/* Vp acquisition */
	pthread_mutex_lock(&lock_vp);
	
	if (testFormat(argv[3], "float")){
		vp.value = atof(argv[3]);
		vp.modif = 1; //data updated
	}
	else{error("getState", "vp");}
	
	/* Test */
	if (in_test == 1){
		printf("getState : recepetion Vp = %f\n", vp.value);
	}
	/////////
	pthread_mutex_unlock(&lock_vp);
	
	
	/* fpa acquisition */
	pthread_mutex_lock(&lock_fpa);
	if (testFormat(argv[4], "float")){
		fpa.value = atof(argv[4]);
		fpa.modif = 1; //data updated
	}
	else{error("getState", "fpa");}
	
	/* Test */
	if (in_test == 1){
		printf("getState : recepetion fpa = %f\n", fpa.value);
	}
	/////////
	pthread_mutex_unlock(&lock_fpa);
	
	
	/*bank_angle_aircraft acquisition*/
	pthread_mutex_lock(&lock_bank_angle_aircraft);

	if (testFormat(argv[6], "float")){
		bank_angle_aircraft.value = atof(argv[6]);
		bank_angle_aircraft.modif = 1; //data updated
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
		printf("Entry into getForecast\n");
	}
	
	pthread_mutex_lock(&lock_TAS);
	if (testFormat(argv[0], "float")){
		tas.value = atof(argv[0]);
		tas.modif = 1; //data updated
	}
	
	else{error("computeBankAngleObj", "tae");}
	pthread_mutex_unlock(&lock_TAS);
	
	pthread_mutex_lock(&lock_Alt);
	if (testFormat(argv[1], "float")){
		alt.value = atof(argv[1]);
		alt.modif = 1; //data updated
	}
	else{error("computeBankAngleObj", "tae");}
	pthread_mutex_unlock(&lock_Alt);
	
	if (in_test == 1){
		printf("getForecast : recepetion TAS = %f, Altitude = %f\n", tas.value, alt.value);
	}
}

//Compute the bank angle wish in order to follow the trajectory
/* function associated with GUID-SEQ information */
void computeBankAngleObj(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	clock_t begin;
	if (in_test == 1){
		printf("Entry into computeBankAngleObj\n");
		begin = clock();
	}
	/////////
	
	//GUID-SEQ data acquisition
	if (testFormat(argv[1], "float")){
		xtk.value = atof(argv[1]) * 1852; 	    //xtk in Nm so conversion to
		xtk.modif = 1; //data updated
	}
	else{error("computeBankAngleObj", "xtk");}
	
	if (testFormat(argv[2], "float")){
		tae.value = atof(argv[2]); 	    //tae in rad
		tae.modif = 1; //data updated
	}
	else{error("computeBankAngleObj", "tae");}
	
	if (testFormat(argv[3], "float")){
		dist.value = atof(argv[3]);	    //dist in m
		dist.modif = 1; //data updated
	}
	else{error("computeBankAngleObj", "dist");}
	
	if (testFormat(argv[4], "float")){
		bank_angle_ref.value = atof(argv[4])* M_PI / 180; //Bank angle arrive en deg 
		bank_angle_ref.modif = 1; //data updated
	}
	else{error("computeBankAngleObj", "bank_angle_ref");}	
		
	/* Test */
    	if (in_test == 1){
    		printf("computeBankAngleNav : reception xtk = %f | tae = %f | dist = %f | bank_angle_ref = %f\n", xtk.value, tae.value, dist.value, bank_angle_ref.value);
    	}
    	/////////
    	/* TODO in test function : Verification of the data temporal trueness
	if(time < (*(variables*)data).param){ 
		fprintf(stderr,"Probleme de chronologie des données\n");
	}*/
	
	float bank_angle_obj; 
	
	pthread_mutex_lock(&lock_gs); //ground speed global variable protection
	

	if (gs.modif){
	
	    pthread_mutex_lock(&lock_ap_mode);
	    int local_ap_mode = ap_mode; //mutex short lock in order to not block in case of mode change
	    pthread_mutex_unlock(&lock_ap_mode);
	
		if(local_ap_mode){
			bank_angle_obj = computeBankAngleObjNav(gs.value); //command computation 
		}
		else if (local_ap_mode == 0){
			pthread_mutex_lock(&lock_heading_aircraft);
			pthread_mutex_lock(&lock_heading_objective);
			bank_angle_obj = computeBankAngleObjHdg();
			heading_aircraft.modif = 0;//data used
			heading_objective.modif = 0;//data used
			pthread_mutex_unlock(&lock_heading_aircraft);
			pthread_mutex_unlock(&lock_heading_objective);
		}
		gs.modif = 0;//data used
		/* Test */
		if (in_test == 1){
			if(local_ap_mode){
				printf("computeBankAngleObjNav : calculation bank_angle_obj_nav = %f\n", bank_angle_obj);
			}
			else if (local_ap_mode == 0){
				printf("computeBankAngleObjHdg : calculation bank_angle_obj_hdg = %f\n", bank_angle_obj);
			}
		}
		/////////
	}
	else{
		bank_angle_obj = 0; //TODO : check if right thing to do
	}
	pthread_mutex_unlock(&lock_gs);
	
	pthread_mutex_lock(&lock_bank_angle_objective);
	global_bank_angle_obj.value = bank_angle_obj;
	global_bank_angle_obj.modif = 1; //data updated
	pthread_mutex_unlock(&lock_bank_angle_objective);
	
	computeCmd(bank_angle_obj);
	
	
	/* Test */
	if (in_test == 1){
	    //computation duration
		clock_t end = clock();
    		unsigned long micro = (end -  begin) * 1000000 / CLOCKS_PER_SEC;
    		printf("Execution time : %ld micro secondes\n",micro);
    	}
    	/////////
}

/* function associated with mode data (NAV or HDG) */
void getMode(IvyClientPtr app, void *data, int argc, char **argv){
	/* Test */
	if (in_test == 1){
		printf("Entry into getMode\n");
	}
	/////////

	char mode_managed[] = "Managed", mode_selected[] = "SelectedHeading";
	
    	pthread_mutex_lock(&lock_ap_mode);
	
	if(strcmp(argv[0],mode_managed) == 0){
	    ap_mode = 1;
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : AP in NAV mode\n");
	    }
	/////////
	}
	else if (strcmp(argv[0],mode_selected) == 0){
	    ap_mode = 0;
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : AP in HDG mode\n");
	    }
	    /////////
	    pthread_mutex_lock(&lock_heading_objective);
	    
	    /* heading_objective acquisition */
	    if (testFormat(argv[1], "int")){
		    heading_objective.value = atol(argv[1]);
		    heading_objective.modif = 1; //data updated useless in this case, but keep the same structure
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
	    ap_mode = 1; //NAV is the default mode
	    
	    /* Test */
	    if (in_test == 1){
		    printf("getMode : AP inactif\n");
		}
	    /////////
	}
	pthread_mutex_unlock(&lock_ap_mode);
}

/* function associated with the clock, used to send datas to aircraft model */
void sendGC(IvyClientPtr app, void *data, int argc, char **argv){
    /* Test */
    if (in_test == 1){
	    printf("Entry into sendGC\n");
	}
    	/////////
    
	char tm[50], rollCommande[100], nxCommande[100], nzCommande[100], apState[100], bankAngle[100], trackObj[100];

	/* Time acquisition */
	if (testFormat(argv[0], "int")){
	    current_time.value = atof(argv[0])*1000; //in ms because easier to deal when cycles will be every 10 ms
	    current_time.modif = 1;
	}
	else{error("sendGC", "current_time");}
	
    if(flight_starting.value == 1){
	    pthread_mutex_lock(&lock_ap_mode);
		int local_ap_mode = ap_mode; //mutex short lock in order to not block in case of mode change
		pthread_mutex_unlock(&lock_ap_mode);
		
		//send AP state and mode every seconds
		if(current_time.value%1000 == 0){
		    if(local_ap_mode == 1){
			sprintf(apState, "GC_AP Time=%ld AP_State='Activated' AP_Mode='NAV'", current_time.value);
		    }
		    else if(local_ap_mode == 0){
			sprintf(apState, "GC_AP Time=%ld AP_State='Activated' AP_Mode='HDG'", current_time.value);
		    }
		IvySendMsg("%s", apState);
		/* Test */
		if (in_test == 1){
			printf("AP state : %d \n", local_ap_mode);
		    }
		  /////////
	    }
	    
	    /////////////////////////////////////////////////////////////////////////////////
	    //Send bankAngleObj
	    /////////////////////////////////////////////////////////////////////////////////
	    pthread_mutex_lock(&lock_bank_angle_objective); //bank_angle_obj protection
	    if(global_bank_angle_obj.modif){
	        global_bank_angle_obj.modif = 0; //data used
	    }
	    else{error("sendGC", "global_bank_angle_obj");}
	    printf("GC_BA Time=%ld Bank_Angle_Obj=%f", current_time.value, global_bank_angle_obj.value);
		sprintf(bankAngle, "GC_BA Time=%ld Bank_Angle_Obj=%f", current_time.value, global_bank_angle_obj.value); //send command, old or updated
		IvySendMsg ("%s", bankAngle);
		pthread_mutex_unlock(&lock_bank_angle_objective);
		
	    /////////////////////////////////////////////////////////////////////////////////
	    //Send roll cmd
	    /////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_roll_cmd); //roll_cmd protection
		if(roll_cmd.modif){
			roll_cmd.modif = 0; //data used
		}
		else {error("sendGC", "roll_cmd");}
		sprintf(rollCommande, "APLatControl rollRate=%f", roll_cmd.value); //send command, old or updated
		IvySendMsg ("%s", rollCommande);
		pthread_mutex_unlock(&lock_roll_cmd);

	 	/* Test */
		if (in_test == 1)
			printf("rollRate= %f time = %ld\n", roll_cmd.value, current_time.value);
		/////////
		pthread_mutex_unlock(&lock_roll_cmd);


		/////////////////////////////////////////////////////////////////////////////////
		//Send nx cmd
		/////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_nx_cmd); // nx_cmd protection
		if (nx_cmd.modif){
			nx_cmd.modif = 0; //data used
		}
		else {error("sendGC", "nx_cmd");}
		sprintf(nxCommande, "APNxControl nx=%f", nx_cmd.value); //send command, old or updated
		IvySendMsg ("%s", nxCommande);
		pthread_mutex_unlock(&lock_nx_cmd);


		/////////////////////////////////////////////////////////////////////////////////
		//Send nz cmd
		/////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_nz_cmd); // nz_cmd protection
		if (nz_cmd.modif){
			nz_cmd.modif = 0; //data used
		}
		else {error("sendGC", "nz_cmd");}
		sprintf(nzCommande, "APNzControl nz=%f", nz_cmd.value); //send command, old or updated
		IvySendMsg ("%s", nzCommande);
		pthread_mutex_unlock(&lock_nz_cmd);
		
		/////////////////////////////////////////////////////////////////////////////////
		//Send track objective
		/////////////////////////////////////////////////////////////////////////////////
		pthread_mutex_lock(&lock_track_obj); // track_obj protection
		if (track_obj.modif){
			track_obj.modif = 0; //data used
		}
		else {error("sendGC", "track_obj");}
		sprintf(trackObj, "GC_TO Time=%ld Track_Obj=%ld", current_time.value, track_obj.value); //send command, old or updated
		IvySendMsg ("%s", trackObj);
		pthread_mutex_unlock(&lock_track_obj);
		
		fin = clock();
		unsigned long micro_tot = (fin -  debut) * 1000000 / CLOCKS_PER_SEC;
	    	printf("Global execution time : %ld micro secondes\n",micro_tot);
	}
	else{
		printf("Aircraft not yet in flight\n");
	}
	
}


/* function associated with  */
void start_flight(IvyClientPtr app, void *data, int argc, char **argv){
	flight_starting.value = 1;
}

/* function associated with  */
void restart(IvyClientPtr app, void *data, int argc, char **argv){
	IvyStop();
}

//Stop Ivy function
void stop(IvyClientPtr app, void *data, int argc, char **argv){
    //kill displays
	int status4 = system("pkill -f horizon_artificiel.py &");
	int status5 = system("pkill -f affichage_commande.py &");
	//int status6 = system("pkill -f affichage_position.py &");
	//kill the application
	exit(EXIT_SUCCESS);
}
/* function associated with ctl+c*/
void intHandler(int dummy) {
    //kill displays
	int status4 = system("pkill -f horizon_artificiel.py &");
	int status5 = system("pkill -f affichage_commande.py &");
	//int status6 = system("pkill -f affichage_position.py &");
	//kill the application
	exit(EXIT_SUCCESS);
}

int start(const char* bus){
    	/* initialisation */
	IvyInit ("GUID_COMM_APP", "Hello from GUID COMM", 0, 0, 0, 0);
	IvyStart (bus);
	

	//we subscribe to
	IvyBindMsg (getPosition, 0, "^AircraftSetPosition X=(.*) Y=(.*) Altitude-ft=(.*) Roll=(.*) Pitch=(.*) Yaw=(.*) Heading=(.*) Airspeed=(.*) Groundspeed=(.*)");
	//AircraftSetPosition X=-2.0366696227720553e-16 Y=0.8693304535637149 Altitude-ft=0.0 Roll=0.0 Pitch=0.0 Yaw=0.0 Heading=360.0 Airspeed=136.06911447084232 Groundspeed=136.06911447084232
	
	IvyBindMsg (getState, 0, "^StateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)");//StateVector x=1610.0 y=-3.7719121413738466e-13 z=0.0 Vp=70.0 fpa=0.0 psi=6.283185307179586 phi=0.0
	
	IvyBindMsg (getForecast, 0, "GT TAS=(.*) CRZ_ALT=(.*)"); //"GT TAS=150.0 CRZ_ALT=1000"
	
  
	IvyBindMsg (computeBankAngleObj, 0, "^GS_Data Time=(.*) XTK=(.*) TAE=(.*) DTWPT=(.*) BANK_ANGLE_REF=(.*) ALDTWPT=(.*)"); 
	

	IvyBindMsg (getMode, 0, "^FCULateral Mode=(.*) Val=(.*)");//FCULateral Mode=SelectedHeading Val=10"
	
	
	// the clock has the role of send synchronization
	IvyBindMsg (sendGC, 0, "^Time t=(.*)");
	
	
	//flight start
	IvyBindMsg (start_flight, 0, "InitStateVector x=(.*) y=(.*) z=(.*) Vp=(.*) fpa=(.*) psi=(.*) phi=(.*)");
	
	 
	IvyBindMsg (restart, 0, "^Restart_GUID_COMM$");
	
	IvyBindMsg (stop, 0, "^Stop_GUID_COMM$");
	
	signal(SIGINT, intHandler); //ctrl+c subscription
	
	// main loop 
	IvyMainLoop();
}

int main (int argc, char**argv){

	int nb_try = 0; //number of times the app has been started (the app will no longer restart if there are too many errors)
	const char* bus = 0;
	
	//Init test
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
			printf("Invalid argument\n");
			exit(1);
		}
	}
	
	/* handling of only -b option */
	else if( argc == 3){
		
		if(strcmp(argv[1], "-b") == 0){
			bus = argv[2];
		}
		else{
			printf("Invalid argument. Define a bus -b 127.127.127.127:2010\n");
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
			printf("Invalid argument. Define a bus -b 127.127.127.127:2010\n");
			exit(1);
		}
	}
	
	else{
		bus = NULL;
	}
	
	/////////////////////////////////////////////////////////////////////////////////
    //IHM launch
    /////////////////////////////////////////////////////////////////////////////////
	int status1 = system("python3 affichage/horizon_artificiel.py &");
	int status2 = system("python3 affichage/affichage_commande.py &");
	//int status3 = system("python3 affichage/affichage_position.py &");
	

    /////////////////////////////////////////////////////////////////////////////////
    // Main loop, allows to restart the application following a crash.
    /////////////////////////////////////////////////////////////////////////////////
	while(nb_try < 4){
	//while(1){
		nb_try++;
		//CAUTION PA in NAV mode at each start
		pthread_mutex_lock(&lock_ap_mode);
		ap_mode = 1;
		pthread_mutex_unlock(&lock_ap_mode);
		//If errors, error function stops the app
		//Resetting errors in the event of a warm restart
		error_init(); 
	    	//call of the main function
    		start(bus);
    		if (in_test == 1)
        		printf("RESTART FUNCTION\n");
	}
	printf("APP CRASHED\n");
	IvySendMsg("GC_AP Time=%ld AP_State='Deactivated' AP_Mode='NULL' ", current_time.value); //TRAJ is notified of the termination of the application
	return 0;
}
