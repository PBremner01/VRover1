#ifndef ARDUINO_CHC_SR04_H
#define ARDUINO_CHC_SR04_H

#include <Arduino.h>

const int trigPin1 = 25;       //Output pin
const int echoPin1 = 35;       //Input pin
const int trigPin2 = 26;       //Output pin
const int echoPin2 = 34;       //Input pin
const int kMaxObstructionDist = 12;      //Maximum distance to obstruction is 12 inches
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701


class CHC_SR04  
{
  private:

	public:
		CHC_SR04();
		~CHC_SR04();
		bool initSR04Cards();
    	float getDistanceFront();
        float getDistanceBack();
		bool isStop(char cDirection);
};

#endif