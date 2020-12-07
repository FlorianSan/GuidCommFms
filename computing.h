#ifndef COMPUTING_H
#define COMPUTING_H

#include "errors.h"
#include "app_guid_comm.h"

#define K1 0.6295 //Gain relatif à la XTK \frac{V_{p}}{g n_{z} \tau_{phi} \tau_{xtk}}
#define K2 0.0105 //Gain relatif à la TAE \frac{V_{p}}{g n_{z} \tau_{phi}}
#define K3 1/3 //Gain relatif à la dynamique de phi

//MACRO
#define min(a,b) (a<=b?a:b) 
#define sgn(x) ((x>0)-(x<0))


//Crée une commande de vitesse de roulis à partir du bank angle souhaité et du bank angle mesuré
void computeRollCmd(float bank_angle_obj, int in_test);

extern float computeBankAngleObjNav(float bank_angle_ref, float xtk, float tae);

//extern float computeBankAngleObjHdg(?);


#endif
