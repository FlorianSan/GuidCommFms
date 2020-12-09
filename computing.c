#include "computing.h"


void computeCmd(float bank_angle_obj, int in_test){
	/* Test */
	if (in_test == 1){
		printf("Entree dans computeCmd\n");
	}
	/////////
	/////////////////////////////////////////////////////////////////////////////////
	//Calcul de roll cmd
	/////////////////////////////////////////////////////////////////////////////////
	float local_bank_angle_aircraft;

	pthread_mutex_lock(&lock_bank_angle_aircraft); // protection de la variable bank_angle_aircraft
	if(bank_angle_aircraft.modif){
		local_bank_angle_aircraft = bank_angle_aircraft.value;
		bank_angle_aircraft.modif = 0;
		/* Test */
		if (in_test == 1)
			printf("computeCmd : mise a jour local_bank_angle_aircraft = %f\n", local_bank_angle_aircraft);
		/////////
	}
	else{
		error("computeCmd", "bank_angle_aircraft", bank_angle_aircraft.modif);
	}
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
	
	float calcul = K3 * (bank_angle_obj - local_bank_angle_aircraft);
	
	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
	roll_cmd.value = calcul;
	roll_cmd.modif = 1;
	/* Test */
	if (in_test == 1){
		printf("computeCmd : calcul roll_cmd = %f\n", roll_cmd.value);
	}
	/////////
	pthread_mutex_unlock(&lock_roll_cmd);
	
	//take fpa
	pthread_mutex_lock(&lock_fpa);
	float fpa_value = fpa.value;
	fpa.modif = 0;
	pthread_mutex_unlock(&lock_fpa);
	
	/////////////////////////////////////////////////////////////////////////////////
	//Calcul de nx cmd
	/////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_nx_cmd); // protection de la variable globale nx_cmd
	nx_cmd.value = sin(fpa_value); // nx = \frac{\dotV}{g} + \sin(\gamma) avec \frac{\dotV}{g} = 0 pour le palier 
	nx_cmd.modif = 1;
	/* Test */
	if (in_test == 1){
		printf("computeCmd : calcul nx_cmd = %f\n", nx_cmd.value);
	}
	/////////
	pthread_mutex_unlock(&lock_nx_cmd);
	
	/////////////////////////////////////////////////////////////////////////////////
	//Calcul de ny cmd
	/////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_nz_cmd); // protection de la variable globale nz_cmd
	pthread_mutex_lock(&lock_bank_angle_aircraft);
	nz_cmd.value = cos(fpa_value)/cos(bank_angle_aircraft.value); //ny = \frac{1}{\cos(\phi)}(\frac{V\dot\gamma}{g} + \cos(\gamma)) avec \frac{V\dot\gamma}{g} = 0 pour le palier 
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
	nz_cmd.modif = 1;
	/* Test */
	if (in_test == 1){
		printf("computeCmd : calcul nz_cmd = %f\n", nz_cmd.value);
	}
	/////////
	pthread_mutex_unlock(&lock_nz_cmd);
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
