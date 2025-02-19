#ifndef __MOTOR_H
#define __MOTOR_H

#include "STC15Fxxxx.H"

#define AIN1 P60
#define AIN2 P61
#define BIN1 P62
#define BIN2 P63
#define CIN1 P64
#define CIN2 P65
#define DIN1 P66
#define DIN2 P67


void Motor_pin_config();
void forward();
void backward();
void stop();


#endif // !__MOTOR_H