#include "motor.h"

void Motor_pin_config() {
	AIN1 = 0;
	AIN2 = 0;
	BIN1 = 0;
	BIN2 = 0;
	CIN1 = 0;
	CIN2 = 0;
	DIN1 = 0;
	DIN2 = 0;

}

void forward() {
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 1;
    BIN2 = 0;
    CIN1 = 1;
    CIN2 = 0;
    DIN1 = 1;
    DIN2 = 0;
}

void backward() {
    AIN1 = 0;
    AIN2 = 1;
    BIN1 = 0;
    BIN2 = 1;
    CIN1 = 0;
    CIN2 = 1;
    DIN1 = 0;
    DIN2 = 1;
}


void stop(){
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
    CIN1 = 0;
    CIN2 = 0;
    DIN1 = 0;
    DIN2 = 0;
}
