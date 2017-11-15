/*
 * barricade.cpp
 *
 * Created on: 2017. 11. 15.
 *      Author: dhrco   
 */

#include <wiringPi.h>
#include <softPwm.h>
#include "barricade.hpp"

Barricade::Barricade(int servo){
    this->servo = servo;

    wiringPiSetup();

    softPwmCreate(servo, 0, 200);
}

void Barricade::open() {
    softPwmWrite(servo, 22);
}

void Barricade::close() {
    softPwmWrite(servo, 15);
}

void Barricade::hold() {
    softPwmWrite(servo, 0);
}
