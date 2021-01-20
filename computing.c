#include "computing.h"


void computeCmd(float bank_angle_obj){
	/* Test */
	if (in_test == 1){
		printf("Entree dans computeCmd \n");
	}
	
	/////////////////////////////////////////////////////////////////////////////////
	//Récupération du bank angle avion
	/////////////////////////////////////////////////////////////////////////////////
	float local_bank_angle_aircraft;
	
	pthread_mutex_lock(&lock_bank_angle_aircraft); // protection de la variable bank_angle_aircraft
		if(bank_angle_aircraft.modif){
			local_bank_angle_aircraft = bank_angle_aircraft.value;
			bank_angle_aircraft.modif = 0;
			/* Test */
			if (in_test == 1){
				printf("computeCmd : mise a jour local_bank_angle_aircraft = %f\n", local_bank_angle_aircraft);
			}
			/////////
		}
		else{error("computeCmd", "local_bank_angle_aircraft");}
	pthread_mutex_unlock(&lock_bank_angle_aircraft);
	
	
	/////////////////////////////////////////////////////////////////////////////////
	//Recupération du fpa
	/////////////////////////////////////////////////////////////////////////////////
	float local_fpa;
	pthread_mutex_lock(&lock_fpa);
		if(fpa.modif){
			local_fpa = fpa.value;
			fpa.modif = 0;
			if (in_test == 1){
				printf("computeCmd : mise a jour local_fpa = %f\n", local_fpa);
			}
		}
		else{error("computeCmd", "local_fpa");}
	pthread_mutex_unlock(&lock_fpa);
	

	/////////////////////////////////////////////////////////////////////////////////
	//Calcul de roll cmd
	/////////////////////////////////////////////////////////////////////////////////
	
	///////////////
	//calcul de K1 -> k_{1} = \frac{1}{\tau_{\phi}}
	float K1 = 1.0/tau_phi;
	
	float calcul = sat(K1 * (bank_angle_obj - local_bank_angle_aircraft), 0.052359); //saturation à 3°/s donc 0.052359 rad/s

	pthread_mutex_lock(&lock_roll_cmd); // protection de la variable globale roll_cmd
		roll_cmd.value = calcul;
		roll_cmd.modif = 1;
		/* Test */
		if (in_test == 1){
			printf("computeCmd : calcul roll_cmd = %f, %d \n", roll_cmd.value, roll_cmd.modif);
		}
		
	pthread_mutex_unlock(&lock_roll_cmd);
	
	
	
	/////////////////////////////////////////////////////////////////////////////////
	//Calcul de nx cmd
	/////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_nx_cmd); // protection de la variable globale nx_cmd
		nx_cmd.value = sat(sin(local_fpa), 1.5); // nx = \frac{\dotV}{g} + \sin(\gamma) avec \frac{\dotV}{g} = 0 pour le palier 
		nx_cmd.modif = 1;
		/* Test */
		if (in_test == 1){
			printf("computeCmd : calcul nx_cmd = %f, %d\n", nx_cmd.value, nx_cmd.modif);
		}
		
	pthread_mutex_unlock(&lock_nx_cmd);

	/////////////////////////////////////////////////////////////////////////////////
	//Calcul de nz cmd
	/////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(&lock_nz_cmd); // protection de la variable globale nz_cmd
		nz_cmd.value = sat(cos(local_fpa)/cos(local_bank_angle_aircraft), 1.5); //ny = \frac{1}{\cos(\phi)}(\frac{V\dot\gamma}{g} + \cos(\gamma)) avec ///		\frac{V\dot\gamma}{g} = 0 pour le palier 
		nz_cmd.modif = 1;
		/* Test */
		if (in_test == 1){
			printf("computeCmd : calcul nz_cmd = %f, %d\n", nz_cmd.value, nz_cmd.modif);
		}
	pthread_mutex_unlock(&lock_nz_cmd);
}

float computeBankAngleObjNav(float ground_speed){

	pthread_mutex_lock(&lock_vp);
	pthread_mutex_lock(&lock_nz_cmd); 
		float K2= - vp.value / (g_gravite * nz_cmd.value * tau_psi);                        //k_{2} = -\frac{V_{p}}{g n_{z}\tau_{\psi}}
		float K3= - vp.value / (g_gravite * nz_cmd.value * ground_speed * tau_xtk * tau_psi);   //k_{3} = -\frac{V_{p}}{g n_{z} G_{s} \tau_{XTK} \tau_{\psi}}
	pthread_mutex_unlock(&lock_nz_cmd);
	pthread_mutex_unlock(&lock_vp);
	
	return sat(bank_angle_ref.value + K2 * tae.value + K3 * xtk.value , 30.0); //Calcul de la commande1

}

float computeBankAngleObjHdg(){
    pthread_mutex_lock(&lock_vp);
	pthread_mutex_lock(&lock_nz_cmd); 
	    float K2= - vp.value / (g_gravite * nz_cmd.value * tau_psi);                        //k_{2} = -\frac{V_{p}}{g n_{z}\tau_{\psi}}
	pthread_mutex_unlock(&lock_nz_cmd);
	pthread_mutex_unlock(&lock_vp);
	
	return sat(K2 * (heading_objective.value - heading_aircraft.value), 30.0);

}
