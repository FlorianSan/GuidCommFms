#ifndef COMPUTING_H
#define COMPUTING_H

#include "errors.h"
#include "pid.h"
#include "app_guid_comm.h"

//MACRO
#define min(a,b) (a<=b?a:b) 
#define sgn(x) ((x>0)-(x<0))


//Crée une commande de vitesse de roulis à partir du bank angle souhaité et du bank angle mesuré
void computeRollCmd(float bank_angle_obj, int in_test);

extern float computeBankAngleObjNav(float bank_angle_ref, float xtk, float tae);

//extern float computeBankAngleObjHdg(?);


#endif
