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
#include <signal.h>
#include <math.h>

#include "computing.h"
#include "errors.h"


pthread_mutex_t lock_roll_cmd;
pthread_mutex_t lock_nx_cmd;
pthread_mutex_t lock_nz_cmd;
pthread_mutex_t lock_bank_angle_aircraft;
pthread_mutex_t lock_bank_angle_objective;
pthread_mutex_t lock_gs;
pthread_mutex_t lock_heading_aircraft;
pthread_mutex_t lock_heading_objective;
pthread_mutex_t lock_fpa;
pthread_mutex_t lock_vp;
pthread_mutex_t lock_ap_mode;
pthread_mutex_t lock_TAS;
pthread_mutex_t lock_Alt;
pthread_mutex_t lock_track_obj;

/*typedef struct variables variables;
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
};*/

typedef struct{
	float value;
	int modif; //1 if data is updated 0 if data is used
}varGlobFloat;

typedef struct{
	long int value;
	int modif; //1 if data is updated 0 if data is used	
}varGlobLint;

//Variables globales protégées des accès concurents
varGlobFloat roll_cmd; // roll command
varGlobFloat nx_cmd; // longitudinal load factor command
varGlobFloat nz_cmd; // vertical load factor command
varGlobFloat global_bank_angle_obj;
varGlobFloat gs; 
varGlobFloat bank_angle_aircraft;
varGlobFloat fpa;
varGlobFloat vp;
//varGlobFloat current_time;
varGlobFloat xtk;
varGlobFloat tae;
varGlobFloat dist;
varGlobFloat bank_angle_ref;
varGlobFloat tas;
varGlobFloat alt;

varGlobLint current_time;
varGlobLint heading_aircraft;
varGlobLint heading_objective;
varGlobLint track_obj;
varGlobLint flight_starting;

clock_t debut;
clock_t fin;

int ap_mode; //PA to protect
int in_test; //global variable of test mode

void getPosition(IvyClientPtr app, void *data, int argc, char **argv);
void getState(IvyClientPtr app, void *data, int argc, char **argv);
void getForecast(IvyClientPtr app, void *data, int argc, char **argv);
void computeBankAngleObj(IvyClientPtr app, void *data, int argc, char **argv);
void getMode(IvyClientPtr app, void *data, int argc, char **argv);
void send(IvyClientPtr app, void *data, int argc, char **argv);
void stop(IvyClientPtr app, void *data, int argc, char **argv);
int start(const char* bus);
int main (int argc, char**argv);

#endif
