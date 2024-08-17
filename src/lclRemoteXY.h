//////////////////////////////////////////////
// RemoteXY connection settings 
#define REMOTEXY_MODE__ESP32CORE_BLE
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"
#include <RemoteXY.h>

// RemoteXY configurate  


/* defined the right motor control pins */
#define RPWM_F 4                     //Right Motors Forward PWM Level , ADC11 , P4
#define RPWM_R 2                    //Right Motors Reverse PWM Level , ADC10 , P16  // Note P0 was not working
#define PIN_MOTOR_RIGHT_SPEED 5
/* defined the left motor control pins */
#define LPWM_F 33                   //Left Motors Forward PWM Level , ADC13, P15
#define LPWM_R 32                    //Left Motors Reverse PWM Level , ADC12 , P2
#define PIN_MOTOR_LEFT_SPEED 5
/* defined two arrays with a list of pins for each motor */
unsigned char RightMotor[3] =
{ RPWM_F, RPWM_R, PIN_MOTOR_RIGHT_SPEED };
unsigned char LeftMotor[3] =
{ LPWM_F, LPWM_R, PIN_MOTOR_LEFT_SPEED };
//           END RemoteXY include          //
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 258 bytes
  { 255,7,0,26,0,251,0,16,13,0,5,10,26,1,51,51,2,26,31,2,
  0,75,55,14,6,2,26,31,31,79,78,0,79,70,70,0,4,0,91,23,
  7,38,2,26,3,2,83,7,6,11,2,26,129,0,80,2,12,4,2,83,
  116,97,116,117,115,0,129,0,90,8,9,3,17,65,119,97,107,101,0,129,
  0,90,14,8,3,17,83,108,101,101,112,0,129,0,2,3,25,4,2,83,
  97,118,101,32,76,97,116,32,76,111,110,0,3,2,3,9,6,10,2,26,
  129,0,2,3,25,4,2,83,97,118,101,32,76,97,116,32,76,111,110,0,
  3,2,3,29,5,9,2,26,129,0,3,23,10,4,2,84,114,97,99,107,
  0,129,0,11,10,9,3,17,79,102,102,0,129,0,10,29,4,3,17,79,
  102,102,0,129,0,11,15,4,3,17,79,110,0,129,0,10,34,4,3,17,
  79,110,0,67,4,8,49,30,5,8,16,13,67,4,8,56,30,5,8,16,
  13,129,0,1,50,6,3,2,76,97,116,58,0,129,0,1,57,7,3,2,
  76,111,110,58,0,129,0,87,22,12,4,17,83,112,101,101,100,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_1_x; // from -100 to 100  
  int8_t joystick_1_y; // from -100 to 100  
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 
  int8_t speed_slider; // =0..100 slider position 
  uint8_t slt_status; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t slt_save_lat_lon; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t slt_track; // =0 if select position A, =1 if position B, =2 if position C, ... 

    // output variables
  char txt_lat[13];  // string UTF8 end zero 
  char txt_lon[13];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

void initRemoteXY()
{
    RemoteXY_Init();
    RemoteXY.joystick_1_x = 0;
    RemoteXY.joystick_1_y = 0;
    RemoteXY.speed_slider = 50;  // initial car speed -25 to 25  
}
