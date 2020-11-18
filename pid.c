#include "pid.h"

#define UMAX 1  //valeur max de roulis
#define UMIN -1 //valeur min de roulis

#define KP 1    // proportional gain
#define KI 0    //integral gain
#define KD 0    //derivative gain

#define N 20    //filter coefficients
#define TS 0.01            

#define A0 (1+N*TS)
#define A1 -(2 + N*TS)
#define A2 1
#define B0 KP*(1+N*TS) + KI*TS*(1+N*TS) + KD*N
#define B1 -(KP*(2+N*TS) + KI*TS + 2*KD*N)
#define B2 KP + KD*N
#define KU1  A1/A0
#define KU2  A2/A0
#define KE0 B0/B0
#define KE1 B1/A0
#define KE2  B2/A0

double e2 = 0;
double e1 = 0;
double u2 = 0;
double u1 = 0;

double pid(float y_plant, float y_c){

	double e0,u0; 
	
	e0 = y_plant - y_c;
	u0 = -KU1*u1 - KU2*u2 + KE0*e0 + KE1*e1 + KE2*e2; 

	if (u0 > UMAX) u0 = UMAX;  // limit 
	if (u0 < UMIN) u0 = UMIN;
	//Déplace les données
	e2 = e1;
	e1 = e0;
	u2 = u1;
	u1 = u0;
	return u0;
}
