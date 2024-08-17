#include <Arduino.h>
#line 1 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
/*
 Name:    RemoteXY_GAccel_V2.ino
 Created: 12/9/2021 5:56:30 PM
 Author:  bremn
*/

#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>
#include <RemoteXY.h>
// Adding Serial Print
#include <SPI.h>
//#include <TinyGPSPLUS.h>
//#include <SoftwareSerial.h>
// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
{ 255,5,0,0,0,85,0,13,13,0,
5,15,26,9,43,43,2,26,31,2,
0,3,2,19,9,2,26,31,31,79,
78,0,79,70,70,0,4,0,83,23,
7,38,2,26,3,2,79,9,6,11,
2,26,129,0,76,4,12,4,2,83,
116,97,116,117,115,0,129,0,86,10,
9,3,17,65,119,97,107,101,0,129,
0,86,16,8,3,17,83,108,101,101,
112,0 };

// this structure defines all the variables and events of your control interface 
struct {

    // input variables
    int8_t joystick_1_x; // =-100..100 x-coordinate joystick position 
    int8_t joystick_1_y; // =-100..100 y-coordinate joystick position 
    uint8_t switch_1; // =1 if switch ON and =0 if OFF 
    int8_t speed_slider; // =0..100 slider position 
    uint8_t slt_status; // =0 if select position A, =1 if position B, =2 if position C, ... 

      // other variable
    uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////
/* defined the LED pin */
#define PIN_LED 27
/* defined the right motor control pins */
//#define RPWM_F 4                     //Right Motors Forward PWM Level , ADC11 , P4
//#define RPWM_R 16                    //Right Motors Reverse PWM Level , ADC10 , P16  // Note P0 was not working
#define RPWM_F 4                     //Right Motors Forward PWM Level , ADC11 , P4
#define RPWM_R 2                    //Right Motors Reverse PWM Level , ADC10 , P16  // Note P0 was not working
#define PIN_MOTOR_RIGHT_SPEED 5
// Enable esp32 sleep mode
#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex

/* defined the left motor control pins */
//#define LPWM_F 15                   //Left Motors Forward PWM Level , ADC13, P15
//#define LPWM_R 2                    //Left Motors Reverse PWM Level , ADC12 , P2
#define LPWM_F 33                   //Left Motors Forward PWM Level , ADC13, P15
#define LPWM_R 32                    //Left Motors Reverse PWM Level , ADC12 , P2
#define PIN_MOTOR_LEFT_SPEED 5
/* defined two arrays with a list of pins for each motor */
unsigned char RightMotor[3] =
{ RPWM_F, RPWM_R, PIN_MOTOR_RIGHT_SPEED };
unsigned char LeftMotor[3] =
{ LPWM_F, LPWM_R, PIN_MOTOR_LEFT_SPEED };

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



#line 79 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
void Wheel(unsigned char* motor0, int x1, unsigned char* motor1, int x2, char direction);
#line 106 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
void Steer(int x1, int y1, int i_speed);
#line 144 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
void Sleep(boolean p_sleep);
#line 151 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
void setup();
#line 173 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
void loop();
#line 79 "c:\\Projects\\Ardruino\\VisualStudioCode\\vrover1\\vrover1.ino"
void Wheel(unsigned char* motor0, int x1, unsigned char* motor1, int x2, char direction)
{
    if (direction == 'F') {             //forward
        analogWrite(motor0[1], 0);
        analogWrite(motor0[0], x1);    //Left Motor  LPWM_F
        analogWrite(motor1[0], 0);
        analogWrite(motor1[1], x2);    //RightMotor  RPWM_R
    Serial.print("Wheel Forward X1; "); Serial.print(x1); Serial.print(" X2: "); Serial.println(x2);
    }
    else if (direction == 'R') {       //Reverse   
        //digitalWrite(motor0[1], HIGH);
        analogWrite(motor0[0], 0);
        analogWrite(motor0[1], x1);    //Left Motor  LPWM_R
        analogWrite(motor1[1], 0);
        analogWrite(motor1[0], x2);    //RightMotor  RPWM_F
    }
    else{                              //Stop
        analogWrite(motor0[0], 0);     //Left Motor
        analogWrite(motor0[1], 0);
        analogWrite(motor1[0], 0);     //RightMotor
        analogWrite(motor1[1], 0);
        Serial.print("Wheel Stop X1; "); Serial.print(x1); Serial.print(" X2: "); Serial.println(x2);
    };

    ; // break point
}

void Steer(int x1, int y1, int i_speed)
{
    bool b_i = (x1 > 0);     bool b_ii = (x1 < 0);     bool b_iii = (y1 > 0);
    char c_fwdrvse;

    // With the RemoteXY.switch off => turn off all engines
    if ((RemoteXY.switch_1 == 0)||
         (( x1 < 5 && x1 > -5) && (y1 < 5 && y1 > -5)))
    {
        Wheel(LeftMotor, 0, RightMotor, 0, 'S');
        //Serial.println("Steer Module exit");
        exit;
    }    

    //For a range of +-15 we have the car go straight forward/backward. 
    if (y1 > 35 || y1 < -35) x1 = y1;

    if (b_iii) c_fwdrvse = 'F'; else c_fwdrvse = 'R';   // For a positive y1 we move forward
    x1 = map(x1, -100, 100, -i_speed, i_speed);
    y1 = map(y1, -100, 100, -i_speed, i_speed);
    
    /* Y coordinate controls speed and cart forward/reverse direction.
       X-Axis control the left/right direction in both forward and reverse movement. */
    switch (c_fwdrvse) {
      case 'F': {
          Wheel(LeftMotor, abs(x1), RightMotor, abs(y1), 'F');
          break;
      };
      case 'R': { //Negative y1 => reverse
          Wheel(LeftMotor, abs(x1), RightMotor, abs(y1), 'R');
          break;
      };
    };  //switch (c_fwdrvse) {

    Serial.print("Out of Steer X1; "); Serial.print(x1); Serial.print(" Y1: "); Serial.print(y1); Serial.print(" Direction: "); Serial.println(c_fwdrvse);

}

void Sleep(boolean p_sleep) {

    if (p_sleep == true) {
        esp_deep_sleep_start();
    };
}

void setup()
{
    RemoteXY_Init();

    // TODO you setup code
    Serial.begin(115200);
    //Setup Right Motors
    pinMode(RPWM_F, OUTPUT);     //Initiates Motor Channel ADC11 Pin, (#0)
    pinMode(LPWM_F, OUTPUT);     //Initiates Motor Channel ADC13 Pin, (#15)
    pinMode(RPWM_R, OUTPUT);     //Initiates Right Motor Direction Enable, pin ADC10 (#4)
    pinMode(LPWM_R, OUTPUT);     //Initiates Left Motor Direction Enable, pin ADC12 (#2)

    RemoteXY.joystick_1_x = 0;
    RemoteXY.joystick_1_y = 0;
    RemoteXY.speed_slider = 25;  // initial car speed -25 to 25

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 1);   //Wake up on pin 17 on HIGH
    
    //Stop all motors
    Wheel(LeftMotor, 0, RightMotor, 0, 'F');
}

void loop()
{
    RemoteXY_Handler();

    //Handle the "Stop" button
    if (RemoteXY.slt_status == true) {
        digitalWrite(PIN_LED, (RemoteXY.slt_status == 1) ? LOW : HIGH);  //Set the ESP board LED pin to on when status is "Sleep"
        Sleep(true);
    };

    // TODO you loop code
    // use the RemoteXY structure for data transfer
    // do not call delay() 

    /* Steer the car */
    Steer(RemoteXY.joystick_1_x, RemoteXY.joystick_1_y, RemoteXY.speed_slider);

}

