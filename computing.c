#include "computing.h"


void computeRollCmd(float bank_angle_obj, int in_test){
	/* Test */
	if (in_test == 1)
		printf("Entree dans computeRollCmd\n");
	/////////

	float local_bank_angle_aircraft;

	pthread_mutex_lock(&lock_bank_angle_aircraft); // protection de la variable bank_angle_aircraft
	if(bank_angle_aircraft.modif){
		local_bank_angle_aircraft = bank_angle_aircraft.value;
		bank_angle_aircraft.modif = 0;
		/* Test */
		if (in_test == 1)
			printf("computeRollCmd : mise a jour local_bank_angle_aircraft = %f\n", local_bank_angle_aircraft);
		/////////
	}
	else{
		error("computeRollCmd/bank_angle_aircraft");
	}
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
	
	float calcul = K3 * (bank_angle_obj - local_bank_angle_aircraft);
	
	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
	roll_cmd.value = calcul;
	roll_cmd.modif = 1;
	/* Test */
	if (in_test == 1)
		printf("computeRollCmd : calcul roll_cmd = %f\n", roll_cmd.value);
	/////////
	pthread_mutex_unlock(&lock_roll_cmd);
}

float computeBankAngleObjNav(float bank_angle_ref, float xtk, float tae){
	float bank_angle_obj_nav;
	float K1=0.6295; //Gain relatif à la XTK \frac{V_{p}}{g n_{z} \tau_{phi} \tau_{xtk}}
	float K2=0.0105; //Gain relatif à la TAE \frac{V_{p}}{g n_{z} \tau_{phi}}
	
	bank_angle_obj_nav = min(bank_angle_ref + K1 * xtk/gs.value + K2 * tae, sgn(bank_angle_ref)*25); //Calcul de la commande1
	
	return bank_angle_obj_nav;
}

float computeBankAngleObjHdg(long int heading_aircraft, long int heading_objective){
	float bank_angle_obj_hdg;
	float K4= 0.1;
	
	bank_angle_obj_hdg = K4 * (heading_objective - heading_aircraft);
	
	return bank_angle_obj_hdg;
}
