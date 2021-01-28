/*
 *	Ivy unbind Test
 *
 *	Copyright (C) 1997-2004
 *	Centre d'�tudes de la Navigation A�rienne
 *
 *	Authors: Yannick Jestin <jestin@cena.fr>
 *
 *	Please refer to file ../src/version.h for the
 *	copyright notice regarding this software
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ivy/ivyloop.h>
#include <Ivy/ivy.h>
#include <Ivy/version.h>
#define REGEXP "."


void HelloCallback (IvyClientPtr app, void *data, int argc, char **argv){
	const char* arg = (argc < 1) ? "" : argv[0];
	IvySendMsg ("Bonjour%s", arg);
}

void Callback (IvyClientPtr app, void *user_data, int argc, char *argv[]) {
	MsgRcvPtr *ptr = (MsgRcvPtr *) user_data;
	printf ("%s sent unbind message, unbinding to %s\n",
 	IvyGetApplicationName(app),REGEXP);
	IvyUnbindMsg(*ptr);
}

int main(int argc, char *argv[]) {
	  MsgRcvPtr ptr;
	  IvyInit("TestUnbind","TestUnbind Ready",NULL,NULL,NULL,NULL);
	  ptr=IvyBindMsg(Callback,&ptr,REGEXP);


	  IvyBindMsg (HelloCallback, 0, "^Hello(.*)");
	  printf("bound to %s\n",REGEXP);
	  IvyStart("10.3.141.120:2010");
	  IvySendMsg("Bonjour de TestUnbind");
	  IvyMainLoop();
	  return 0;
}
