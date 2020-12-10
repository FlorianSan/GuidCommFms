#ifndef COMPUTING_H
#define COMPUTING_H

#include "errors.h"
#include "app_guid_comm.h"
#include <math.h>

#define tau_phi 3
#define tau_xtk 3

#define K3 1/3 //Gain relatif à la dynamique de phi

//MACRO
#define min(a,b) (a<=b?a:b) 
#define sgn(x) ((x>0)-(x<0))


//Crée une commande de vitesse de roulis à partir du bank angle souhaité et du bank angle mesuré
void computeCmd(float bank_angle_obj);

extern float computeBankAngleObjNav();

extern float computeBankAngleObjHdg();


#endif
