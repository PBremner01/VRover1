#include "CHC_SR04.hpp"

    CHC_SR04::CHC_SR04(){
    }

	CHC_SR04::~CHC_SR04(){
     }

    bool CHC_SR04::initSR04Cards(){
        Serial.println( "init SR04Cards");
        pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
        pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
        pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
        pinMode(echoPin2, INPUT); // Sets the echoPin as an Input        Serial.println(" --------------------");
        return true;
    }

    float CHC_SR04::getDistanceFront(){
        long duration;
        float distanceCm;
        float distanceInch;
        
       // Clears the trigPin
        digitalWrite(trigPin1, LOW);
        delayMicroseconds(2);
        // Sets the trigPin on HIGH state for 10 micro seconds
        digitalWrite(trigPin1, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin1, LOW);
        
        // Reads the echoPin, returns the sound wave travel time in microseconds
        duration = pulseIn(echoPin1, HIGH);
        
        // Calculate the distance
        distanceCm = duration * SOUND_SPEED/2;
        
        // Convert to inches
        distanceInch = distanceCm * CM_TO_INCH;
        //Serial.print("CHC_SR04::getDistanceFront distanceInch: "); Serial.println(distanceInch);        
        return distanceInch;
    }

    float CHC_SR04::getDistanceBack(){
        long duration;
        float distanceCm;
        float distanceInch;
        
        // Clears the trigPin
        digitalWrite(trigPin2, LOW);
        delayMicroseconds(2);
        // Sets the trigPin on HIGH state for 10 micro seconds
        digitalWrite(trigPin2, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin2, LOW);
        
        // Reads the echoPin, returns the sound wave travel time in microseconds
        duration = pulseIn(echoPin2, HIGH);
        
        // Calculate the distance
        distanceCm = duration * SOUND_SPEED/2;
        
        // Convert to inches
        distanceInch = distanceCm * CM_TO_INCH;
        //Serial.print("CHC_SR04::getDistanceBack distanceInch: "); Serial.println(distanceInch);
        return distanceInch;
    }

    bool CHC_SR04::isStop(char cDirection){
        if ( cDirection == 'F'){
           if ( getDistanceFront() <= kMaxObstructionDist) return true;
        }
        else if ( cDirection == 'B'){ 
           if ( getDistanceBack() <= kMaxObstructionDist) return true;
        }
        return false;
    }