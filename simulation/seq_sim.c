#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ivy.h>
#include <ivyloop.h>
#include <string.h>
#include <unistd.h>



void fonction(IvyClientPtr app, void *data, int argc, char **argv){
	float time = atof(argv[0]);
	usleep(10);
	IvySendMsg ("GS_Data Time=%f XTK=%f TAE=%f DTWPT=%f BANK_ANGLE_REF=%f", time, 0.0, 0.0, 1.0, 0.1);
}


int main (int argc, char**argv){

	const char* bus = 0;

	//Initialisation test
	int in_test = 0;
	
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
	IvyInit ("Sim_GuidSeq", "Bonjour de sim guid seq", 0, 0, 0, 0);
	IvyStart (bus);
	
	//on s'abonne à l'holorge qui cadence nos envois
	IvyBindMsg (fonction, 0, "^Time t=(.*)");
	
	/* main loop */
	IvyMainLoop();
	return 0;
}
