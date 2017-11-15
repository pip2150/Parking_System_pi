/*
 * lcd.cpp
 *
 * Created on: 2017. 11. 15.
 *      Author: dhrco   
 */

#include <cstdio>                                                            //include standard input output head file                                                            
#include <cstring>
#include <wiringPi.h>                                                         //include wiringpi                                                     
#include <mcp23017.h>                                                         //include mcp23017 control head file                                         
#include <lcd.h>                                                              //include LCD control head file                                                   
#include <softPwm.h>                                                          //include PWM control head file
#include "lcd.hpp"

LCD::LCD(){
    wiringPiSetup();                                                      //init wiringPi                                                    
    mcp23017Setup (100, 0x20);                                            //init mcp23017 chip I2C address: 0x20,the first pin number: 100                                          

    for(int i = 0 ; i < 16 ; i++ )                                                         
        pinMode(100+i,OUTPUT);                                                //set pin 100 - 115 as output                          

    digitalWrite(101,0);                                                  //set pin 101 low voltage                              
    disp = lcdInit(2,16,4,100,102,103,104,105,106,0,0,0,0);              //lcd init 2*16,4 bit control,use 100,101,102,103,104 pin as control pin         

    pinMode(0, OUTPUT);                                                   //set Raspberry pi pin 0 as output   
    pinMode(2, OUTPUT);                                                   //set Raspberry Pi pin 2 as output  
    pinMode(3, OUTPUT);                                                   //set Raspberry Pi pin 3 as output

    softPwmCreate (3, 50, 100);                                           //set soft PWM pin 3 PWM scale (0-100) original 50
    softPwmCreate (2, 50, 100);                                           //set soft PWM pin 2 PWM scale (0-100) original 50
    softPwmCreate (0, 50, 100);                                           //set soft PWM pin 0 PWM scale (0-100) original 50
}

LCD::~LCD(){

    lcdHome(disp);                                                     //reset cursor  
    lcdClear(disp);                                                    //clear screen    
    digitalWrite(101,1);                                                  //set pin 101 low voltage                              

}

void LCD::display(char topText[], char bottomText[], RGB rgb){

    lcdHome(disp);                                                     //reset cursor  
    lcdClear(disp);                                                    //clear screen    
    lcdPosition(disp, 2, 0);
    lcdPuts(disp, topText);
    lcdPosition(disp, 2, 1);
    lcdPuts(disp, bottomText);

    softPwmWrite (3,rgb.r);                                             //soft PWM control red backlight
    softPwmWrite (2,rgb.g);                                           //soft PWM control green backlight
    softPwmWrite (0,rgb.b);                                            //soft PWM control blue backlight
}
