#include "errors.h"


//Gère les erreurs récurrentes
void error(char* function, char* param, int nb_errors){
	fprintf(stderr,"Problem in %s about %s\n",function, param);
	if (strcmp(function, "acquisition"))
	{
		//TODO envoyer un message à seq?
		if (nb_errors == 3)
		{
			// voir si il faut remettre le compteur à 0?
			ap_state = -1;
			//Il est possible de faire un redemarage à chaud grace a un commande IvyStop(); //(florian)
			//on envoie où qu'on à désactivé le PA?
		}
	}
	else if (strcmp(function, "computeCmd") || strcmp(function, "computeBankAngleObjNav"))
	{
		if (nb_errors == -3)
			ap_state = -1;
	}	
}

//Transforme les données récupérées au format souhaité et informe en cas d'erreur
void acquisition(void *data, char** argv){
	/* Récupération heure actuelle */
	if (testFormat(argv[0], "float"))
	{
		(*(variables*)data).time = atof(argv[0]);
		(*(variables*)data).cpt_time = 0;
	}
	else
		(*(variables*)data).cpt_time++;
		error("acquisition", "time", (*(variables*)data).cpt_time);
		
	/* Récupération xtk */
	if (testFormat(argv[1], "float"))
	{
		(*(variables*)data).xtk = atof(argv[1]);
		(*(variables*)data).cpt_xtk = 0;
	}
	else
		(*(variables*)data).cpt_xtk++;
		error("acquisition", "xtk", (*(variables*)data).cpt_xtk);
		
	/* Récupération tae */
	if (testFormat(argv[2], "float"))
	{
		(*(variables*)data).tae = atof(argv[2]);
		(*(variables*)data).cpt_tae = 0;
	}
	else
		(*(variables*)data).cpt_tae++;
		error("acquisition", "tae", (*(variables*)data).cpt_tae);
		
	/* Récupération dtwpt */
	if (testFormat(argv[3], "float"))
	{
		(*(variables*)data).dist = atof(argv[3]);
		(*(variables*)data).cpt_dist = 0;
	}
	else
		(*(variables*)data).cpt_dist++;
		error("acquisition", "dist", (*(variables*)data).cpt_dist);
		
	/* Récupération bank angle reference */
	if (testFormat(argv[4], "float"))
	{
		(*(variables*)data).bank_angle_ref = atof(argv[4]);
		(*(variables*)data).cpt_bank_angle_ref = 0;
	}
	else
		(*(variables*)data).cpt_bank_angle_ref++;
		error("acquisition", "bank_angle_ref", (*(variables*)data).cpt_bank_angle_ref);	
}

//Teste si le format de réception est correct
int testFormat(char* c, char* type){
	int length = strlen(c);
	int cpt_point = 0; //il ne faut qu'un seul point dans un float
	char temp[1]; //tampon pour le caractère en cours d'analyse (nécessaire pour le .)
	for (int j=0; j<length; j++)
	{
		strncpy(temp,&c[j],1);
		if (! isdigit(c[j]))
		{
			if (strcmp(".", temp) == 0)
			{
				cpt_point++;
			}
			else return 0;
		}		
				
	}
	if ((cpt_point > 0 && type == "int") || (cpt_point > 1 && type == "float"))
		return 0;
	return 1;
}
