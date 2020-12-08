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
pthread_mutex_t lock_nx_cmd;
pthread_mutex_t lock_nz_cmd;
pthread_mutex_t lock_bank_angle_aircraft;
pthread_mutex_t lock_gs;
pthread_mutex_t lock_heading_aircraft;
pthread_mutex_t lock_heading_objective;
pthread_mutex_t lock_fpa;

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
struct varGlobFloat nx_cmd; // commande en facteur de charge longitudinal
struct varGlobFloat nz_cmd; // commande en facteur de charge vertical
struct varGlobFloat global_bank_angle_obj;
struct varGlobFloat gs; //ground speed
struct varGlobFloat bank_angle_aircraft;
struct varGlobFloat fpa;

struct varGlobLint heading_aircraft;
struct varGlobLint heading_objective;

void getPosition(IvyClientPtr app, void *data, int argc, char **argv);
void getState(IvyClientPtr app, void *data, int argc, char **argv);
void computeBankAngleObj(IvyClientPtr app, void *data, int argc, char **argv);
void getMode(IvyClientPtr app, void *data, int argc, char **argv);
void sendCmd(IvyClientPtr app, void *data, int argc, char **argv);
void stop(IvyClientPtr app, void *data, int argc, char **argv);
int main (int argc, char**argv);

#endif
