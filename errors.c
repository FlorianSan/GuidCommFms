#include "errors.h"


//Gère les erreurs récurrentes
void error(char* function, char* param){
	fprintf(stderr,"ERROR : Problem in %s about %s\n",function, param);
	
	//gestion d'une seule erreur
	if (strcmp(param, "roll_cmd")){
		if (roll_cmd.modif == 1)
			roll_cmd.modif = -1;
		else roll_cmd.modif--;
		if (roll_cmd.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value); //on prévient TRAJ de la désactivation du PA
			IvyStop(); //arrêt de l'app, redémarrage à chaud
		}
	}
	else if (strcmp(param, "nx_cmd")){
		if (nx_cmd.modif == 1)
			nx_cmd.modif = -1;
		else nx_cmd.modif--;
		if (nx_cmd.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "nz_cmd")){
		if (nz_cmd.modif == 1)
			nz_cmd.modif = -1;
		else nz_cmd.modif--;
		if (nz_cmd.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "global_bank_angle_obj")){
		if (global_bank_angle_obj.modif == 1)
			global_bank_angle_obj.modif = -1;
		else global_bank_angle_obj.modif--;
		if (global_bank_angle_obj.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "gs")){
		if (gs.modif == 1)
			gs.modif = -1;
		else gs.modif--;
		if (gs.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop(); 
		}
	}
	else if (strcmp(param, "bank_angle_aircraft")){
		if (bank_angle_aircraft.modif == 1)
			bank_angle_aircraft.modif = -1;
		else bank_angle_aircraft.modif--;
		if (bank_angle_aircraft.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "fpa")){
		if (fpa.modif == 1)
			fpa.modif = -1;
		else fpa.modif--;
		if (fpa.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "xtk")){
		IvySendMsg ("GC_ER XTK");
		if (xtk.modif == 1)
			xtk.modif = -1;
		else xtk.modif--;
		if (xtk.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "tae")){
		IvySendMsg ("GC_ER TAE");
		if (tae.modif == 1)
			tae.modif = -1;
		else tae.modif--;
		if (tae.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "dist")){
		IvySendMsg ("GC_ER DTWPT");
		if (dist.modif == 1)
			dist.modif = -1;
		else dist.modif--;
		if (dist.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "bank_angle_ref")){
		IvySendMsg ("GC_ER BANK_ANGLE_REF");
		if (bank_angle_ref.modif == 1)
			bank_angle_ref.modif = -1;
		else bank_angle_ref.modif--;
		if (bank_angle_ref.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "current_time")){
		if (current_time.modif == 1)
			current_time.modif = -1;
		else current_time.modif--;
		if (current_time.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "heading_aircraft")){
		if (heading_aircraft.modif == 1)
			heading_aircraft.modif = -1;
		else heading_aircraft.modif--;
		if (heading_aircraft.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}
	else if (strcmp(param, "heading_objective")){
		if (heading_objective.modif == 1)
			heading_objective.modif = -1;
		else heading_objective.modif--;
		if (heading_objective.modif == -3){
			IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
			IvyStop();
		}
	}

	

	//gestion d'erreurs simultanées
	int cpt; //Nombre d'erreurs simultanées
	cpt = (roll_cmd.modif < 0) + (nx_cmd.modif < 0) + (nz_cmd.modif < 0) + (global_bank_angle_obj.modif < 0) + (gs.modif < 0) + (bank_angle_aircraft.modif < 0) + (fpa.modif < 0) + (xtk.modif < 0) + (tae.modif < 0) + (dist.modif < 0) + (current_time.modif < 0) + (bank_angle_ref.modif < 0) + (heading_aircraft.modif < 0) + (heading_objective.modif < 0);
	if (cpt > 1){ //Au delà de deux erreurs simultanées, on désactive le PA
	   	IvySendMsg("GC_AP Time=%ld AP_State='Restart'", current_time.value);
		IvyStop();
	}
}

/*int acquisition(void* data, char* arg, char* type){
	if (strcmp(type, "float")){
		printf("arg : %s, test format : %d\n", arg, testFormat(arg, "float"));
		if (testFormat(arg, "float")){
			(*(varGlobFloat*)data).value = atof(arg);
			printf("arg : %s, nouvelle valeur : %f\n", arg, (*(varGlobFloat*)data).value);
			(*(varGlobFloat*)data).modif = 1;
		}
		else{
			if ((*(varGlobFloat*)data).modif == 1)
				(*(varGlobFloat*)data).modif = -1;
			else (*(varGlobFloat*)data).modif--;
			return 0;
		}
	}
	else if (strcmp(type, "int")){
		if (testFormat(arg, "int")){
			(*(varGlobLint*)data).value = atol(arg);
			(*(varGlobLint*)data).modif = 1;
		}
		else{
			if ((*(varGlobLint*)data).modif == 1)
				(*(varGlobLint*)data).modif = -1;
			else (*(varGlobLint*)data).modif--;
			return 0;
		}
	}
	return 1;
}*/

//Teste si le format de réception est correct
int testFormat(char* c, char* type){
	int length = strlen(c);
	int point_cpt = 0; //il ne faut qu'un seul point dans un float
	int minus_cpt = 0; //les caractères peuvent être négatifs
	int scient_cpt = 0; //ecriture scientifique
	int bool_scient = 0;
	char temp[1]; //tampon pour le caractère en cours d'analyse (nécessaire pour le "." et le "-")
	for (int j=0; j<length; j++){
		strncpy(temp,&c[j],1);
		if (! isdigit(c[j])){
			
			if (strcmp(".", temp) == 0){
				point_cpt++;
			}
			else if (strcmp("e", temp) == 0 || strcmp("+", temp) == 0 ){
			    if(strcmp("e", temp) == 0){
			        bool_scient = 1;
			    }
			    scient_cpt++;
			}
			else if (strcmp("-", temp) == 0){
				minus_cpt++;
			}
			else return 0;
		}					
	}
	if ((point_cpt > 0 && type == "int") || (point_cpt > 1 && type == "float") || (minus_cpt > 1) && (bool_scient == 0) || (point_cpt > 1 && scient_cpt > 2 && (minus_cpt > 2) && (bool_scient == 1) && type == "float"))
		return 0;
	return 1;
}

//Réinitialise les compteurs d'erreur (TODO si temps : démarrage à froid)
void error_init(){
	gs.modif = 0;
	bank_angle_aircraft.modif = 0;
	fpa.modif = 0;
	current_time.modif = 0;
	xtk.modif = 0;
	tae.modif = 0;
	dist.modif = 0;
	bank_angle_ref.modif = 0;
	heading_aircraft.modif = 0;
	heading_objective.modif = 0;
}
