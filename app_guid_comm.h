#ifndef APP_GUID_COMM_H
#define APP_GUID_COMM_H

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ivy.h>
#include <ivyloop.h>
#include <errno.h>  
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "computing.h"
#include "errors.h"


pthread_mutex_t lock_roll_cmd;
pthread_mutex_t lock_bank_angle_aircraft;
pthread_mutex_t lock_gs;
pthread_mutex_t lock_heading_aircraft;
pthread_mutex_t lock_heading_objective;

typedef struct variables variables;
struct variables{
    int test;
    float param;  
};

struct varGlobFloat
{
	float value;
	int modif; //1 si l'info est produite 0 si l'info est consommé	
};

struct varGlobLint
{
	long int value;
	int modif; //1 si l'info est produite 0 si l'info est consommé	
};

//Variables globales protégées des accès concurents
struct varGlobFloat roll_cmd; // commande de roulis
struct varGlobFloat global_bank_angle_obj;
struct varGlobFloat gs; //ground speed
struct varGlobFloat bank_angle_aircraft;

struct varGlobLint heading_aircraft;
struct varGlobLint heading_objective;


#endif
