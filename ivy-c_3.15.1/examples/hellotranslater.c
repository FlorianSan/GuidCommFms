#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ivy.h>
#include <ivyloop.h>
typedef struct Coordonnees Coordonnees;
struct Coordonnees
	{
	    int x; // Abscisses
	    int y; // Ordonnées
	};
	
/* callback associated to "Hello" messages */
void HelloCallback (IvyClientPtr app, void *data, int argc, char **argv){
	fprintf(stderr,"%d\n",(*(Coordonnees*)data).x);
	
}



/* callback associated to "Bye" messages */
void ByeCallback (IvyClientPtr app, void *data, int argc, char **argv){
	IvyStop ();
}
int main (int argc, char**argv){
	
	struct Coordonnees point;
	point.x = 10;
   	point.y = 20;
	/* initializations */
	IvyInit ("IvyTranslater", "Hello le monde", 0, 0, 0, 0);
	IvyStart (NULL);
	/* binding of HelloCallback to messages starting with 'Hello' */
	IvyBindMsg (HelloCallback, &point, "^hello"); 
	/* binding of ByeCallback to 'Bye' */
	IvyBindMsg (ByeCallback, 0, "^Bye$");
	/* main loop */
	IvyMainLoop();
	return 0;
}
