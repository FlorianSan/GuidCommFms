#include "errors.h"


//Gère les erreurs récurrentes
void error(char* info){
	fprintf(stderr,"Probleme %s\n",info); //voir si signal peut faire le job
}

//Teste si le format de réception est correct
/*bool testFormat(?){
}*/
