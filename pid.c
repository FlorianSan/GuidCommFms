#include "pid.h"


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
