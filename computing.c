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
	
	float calcul = pid(bank_angle_obj, local_bank_angle_aircraft); //calcul du pid coeff défini dans pid.c
	
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
	const float k1 = -0.0044;
	const float k2 = -1.5;
	
	//TODO calculer le K2 pour un tae/gs
	bank_angle_obj_nav = min(bank_angle_ref + k1 * xtk + k2 * tae, sgn(bank_angle_ref)*25); //Calcul de la commande 
	//bank_angle_obj_nav = min(bank_angle_ref + k1 * xtk + k2 * tae/gs.value, sgn(bank_angle_ref)*25); //Calcul de la commande
	return bank_angle_obj_nav;
}

/*float computeBankAngleObjHead(?){
	float bank_angle_obj_hdg;
	
	//TODO calcul en HDG
	
	return bank_angle_obj_hdg;
}*/
