
#ifndef ARDUINO_CSTEER_H
#define ARDUINO_CSTEER_H
#include <Arduino.h>
#include <analogWrite.h>
#include "CHC_SR04.hpp"


/* defined the right motor control pins */
#define RPWM_F 4                      //Right Motors Forward PWM Level , ADC10 , P4
#define RPWM_R 2                      //Right Motors Reverse PWM Level , ADC12 , P2  
#define PIN_MOTOR_RIGHT_SPEED 5

/* defined the left motor control pins */
#define LPWM_F 33                     //Left Motors Forward PWM Level , ADC5, P33
#define LPWM_R 32                     //Left Motors Reverse PWM Level , ADC4 ,P32
#define PIN_MOTOR_LEFT_SPEED 5


class CSteer  
{
	private:
		/* defined two arrays with a list of pins for each motor */
		unsigned char RightMotor[3] =
		{ RPWM_F, RPWM_R, PIN_MOTOR_RIGHT_SPEED };
		unsigned char LeftMotor[3] =
		{ LPWM_F, LPWM_R, PIN_MOTOR_LEFT_SPEED };
		char c_Direction;
		int i_speed = 60;
	public:

		CSteer(); 		
		~CSteer();
		void SetDirection(char direction);
		char GetDirection();
		void SetSpeed(int speed);
		int GetSpeed();
		void Steer(int x1, int y1, CHC_SR04 oSR04);
		void Forward( int i_speed, CHC_SR04 oSR04 );
		void Back( int i_speed, CHC_SR04 oSR04 );
		void Left( int i_speed, int i_tlevel, CHC_SR04 oSR04 );
		void Right( int i_speed, int i_tlevel, CHC_SR04 oSR04);
		void Wheel(unsigned char* motor0, int x1, unsigned char* motor1, int x2, char direction);
		void Stop();
};
#endif
