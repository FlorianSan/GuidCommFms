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
pthread_mutex_t lock_ap_state;

typedef struct variables variables;
struct variables{ //structure pour faire passer les infos dans computeBankAngleObj
    float time;
    int cpt_time;
    float xtk;
    int cpt_xtk;
    float tae;
    int cpt_tae;
    float dist;
    int cpt_dist;
    float bank_angle_ref;
    int cpt_bank_angle_ref;
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

int ap_state; //PA à protéger
int in_test; //variable globale du mode test

void getPosition(IvyClientPtr app, void *data, int argc, char **argv);
void getState(IvyClientPtr app, void *data, int argc, char **argv);
void computeBankAngleObj(IvyClientPtr app, void *data, int argc, char **argv);
void getMode(IvyClientPtr app, void *data, int argc, char **argv);
void send(IvyClientPtr app, void *data, int argc, char **argv);
void stop(IvyClientPtr app, void *data, int argc, char **argv);
int start(const char* bus, float sendCmd, struct variables varComputeBankAngleObj);
int main (int argc, char**argv);

#endif
