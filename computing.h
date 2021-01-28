#ifndef COMPUTING_H
#define COMPUTING_H

#include "errors.h"
#include "app_guid_comm.h"
#include <math.h>
/*
#define tau_phi 3.0 
#define tau_psi 10.0
#define tau_xtk 3.0
*/
#define tau_phi 30.0 
#define tau_psi 50.0
#define tau_xtk 30.0

#define g_gravite 9.81


//MACRO
#define sat(x,sat_val) ( ((x) > sat_val) ? sat_val : ( (-(x) > sat_val) ? (-sat_val) : (x) ) ) //macro de stauration symetrique



//Crée une commande de vitesse de roulis à partir du bank angle souhaité et du bank angle mesuré
void computeCmd(float bank_angle_obj);

extern float computeBankAngleObjNav(float ground_speed);

extern float computeBankAngleObjHdg();


#endif
