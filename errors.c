#include "errors.h"


//Gère les erreurs récurrentes
void error(char* function, char* param){
	fprintf(stderr,"Problem in %s about %s\n",function, param);
	//TODO envoyer message à SEG
	
	//gestion d'une seule erreur
	/*if (strcmp(param, "gs")){
		if (gs.modif == 1)
			gs.modif = -1;
		else gs.modif--;
		if (gs.modif == -3)
			ap_state = -1;//TODO créer fonction qui désactive le PA et redémarre à chaud
	}
	
	
	//gestion d'erreur simultanées
	int cpt; //Nombre d'erreurs simultanées
	if (gs.modif < 0)
		cpt++;
	if (bank_angle_aircraft.modif < 0)
		cpt++;
	if (fpa.modif < 0)
		cpt++;
	if (current_time.modif < 0)
		cpt++;
	if (xtk.modif < 0)
		cpt++;
	if (tae.modif < 0)
		cpt++;
	if (dist.modif < 0)
		cpt++;
	if (bank_angle_ref.modif < 0)
		cpt++;
	if (heading_aircraft.modif < 0)
		cpt++;
	if (heading_objective.modif < 0)
		cpt++;
	if (cpt > 1) //Au delà de deux erreurs simultanées, on désactive le PA
		ap_state = -1;*/
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
	int cpt_point = 0; //il ne faut qu'un seul point dans un float
	char temp[1]; //tampon pour le caractère en cours d'analyse (nécessaire pour le .)
	for (int j=0; j<length; j++){
		strncpy(temp,&c[j],1);
		if (! isdigit(c[j])){
			if (strcmp(".", temp) == 0)
				cpt_point++;
			else return 0;
		}				
	}
	if ((cpt_point > 0 && type == "int") || (cpt_point > 1 && type == "float"))
		return 0;
	return 1;
}
