#ifndef PID_H
#define PID_H

#define UMAX 3  //valeur max de roulis
#define UMIN -3 //valeur min de roulis

#define KP 7    // proportional gain
#define KI 0    //integral gain
#define KD 12    //derivative gain

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
#define KE0 B0/A0
#define KE1 B1/A0
#define KE2  B2/A0


extern double pid(float y_plant, float y_c);


#endif
