#include "CSteer.hpp"  


    CSteer::CSteer()
    {
        pinMode(RPWM_F, OUTPUT);     //Initiates Motor Channel ADC10 Pin, (#4)
        pinMode(LPWM_F, OUTPUT);     //Initiates Motor Channel ADC5 Pin, (#33)
        pinMode(RPWM_R, OUTPUT);     //Initiates Right Motor Direction Enable, pin ADC12 (#2)
        pinMode(LPWM_R, OUTPUT);     //Initiates Left Motor Direction Enable, pin ADC4 (#32)
    }

    CSteer::~CSteer()
    { }

    char CSteer::GetDirection(){
        return c_Direction;
    }

    void CSteer::SetDirection(char direction){
        c_Direction = direction;
    }

    int CSteer::GetSpeed(){
        return i_speed;
    }

    void CSteer::SetSpeed(int speed){
        i_speed = speed;
    }
    void CSteer::Steer(int x1, int y1, CHC_SR04 oSR04)
    {
        Serial.print("In CSteer::Steer X: "); Serial.print(x1); Serial.print(" Y1: "); Serial.println( y1); 
    
        bool b_iii = (y1 > 0);
        char c_fwdrvse;   

        if (b_iii) c_fwdrvse = 'F'; 
        else c_fwdrvse = 'R';   // For a positive y1 we move forward

        if ( oSR04.isStop(c_fwdrvse) ){   //Obstruction sensors initiate a Stop
            Stop();
            Serial.println("Steer oSR04 isStop exit");
            return;
        }

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
        };  //switch (c_fwdrvse) 

   }

   void CSteer::Forward(int i_speed, CHC_SR04 oSR04)
   {
       Steer(i_speed, i_speed, oSR04);
   }

   void CSteer::Back(int i_speed, CHC_SR04 oSR04)
   {
       Steer(i_speed, -i_speed, oSR04);  //Negative Y value => back
   }
      void CSteer::Left(int i_speed, int i_tlevel, CHC_SR04 oSR04)
   {  // Left wheel (x), turns less than right wheel
       int x = i_speed * (double(i_tlevel)/100.0);
       Serial.print("Left: "); Serial.println( x );
       int y = i_speed;
       Steer(x, y, oSR04);
   }

   void CSteer::Right(int i_speed, int i_tlevel, CHC_SR04 oSR04)
   {  // Right wheel (y), turn less than left wheel
       int x = i_speed;
       int y = i_speed * (double(i_tlevel)/100.0);
       Steer(x, y, oSR04);
   }

    void CSteer::Wheel(unsigned char* motor0, int x1, unsigned char* motor1, int x2, char direction)
    {
        if (direction == 'F') {             //forward
            analogWrite(motor0[1], 0);
            analogWrite(motor0[0], x1);    //Left Motor  LPWM_F
            analogWrite(motor1[0], 0);
            analogWrite(motor1[1], x2);    //RightMotor  RPWM_R
        }
        else if (direction == 'R') {       //Reverse   
            analogWrite(motor0[0], 0);
            analogWrite(motor0[1], x1);    //Left Motor  LPWM_R
            analogWrite(motor1[1], 0);
            analogWrite(motor1[0], x2);    //RightMotor  RPWM_F
        }
        else {                              //Stop
            analogWrite(motor0[0], 0);     //Left Motor
            analogWrite(motor0[1], 0);
            analogWrite(motor1[0], 0);     //RightMotor
            analogWrite(motor1[1], 0);
        };
    }

    void CSteer::Stop()
    {
        Wheel(LeftMotor, 0, RightMotor, 0, 'F');         
    }



