
#ifndef ARDUINO_CSTEER_H
#define ARDUINO_CSTEER_H
#include <Arduino.h>
#include <analogWrite.h>

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
	public:

		CSteer();
		~CSteer();
		void Steer(int x1, int y1, int i_speed, bool b_on);
		void Wheel(unsigned char* motor0, int x1, unsigned char* motor1, int x2, char direction);
};
#endif
