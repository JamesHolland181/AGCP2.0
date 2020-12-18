//
// -*- mode: C++ -*-
//
// Running the motor smoothly with accelstepper library
// 6-11-2012
// fixed button issue after 5!

#include <AccelStepper.h>
#include <Bounce.h> // debouncing

// INPUTS
#define PowerButt 2 // On/Off button
#define IncButt 3 // increment button
#define DecButt 4 // decrement button

Bounce bouncerON = Bounce( PowerButt, 50 ); // debouncing
Bounce bouncerINC = Bounce( IncButt, 1 );
Bounce bouncerDEC = Bounce( DecButt, 1 );

// OUTPUTS
#define LitePin 5 // pin for light-up button LEDs
#define DisplayPin 6 // pin for Rx on lcd display

#define DIR_PIN 7 // direction pin on Big EasyDriver from SparkFun
#define STEP_PIN 9 // Step pin on Big EasyDriver
AccelStepper stepperB(1, STEP_PIN, DIR_PIN);


// Variables
int MotorRun = 0; // TRUE or FALSE for if the mnotor is moving or not
int MotorSpeed = 0; // integer RPM (rotations per minute)

int SetMotorSpeed = 3200; // The motor speed that is desired (SETPOINT)

int RateInc = 5; // Amount (in RPMs) that the motor speed is incremented each time a decrement button is pressed
int StartUp = 1; // TRUE or FALSE if it is starting up
int MaxAccel = 1000; // Used by AccelStepper for accelleration

int count = 0; // counter for debuggggg
int buttonTime = millis();

void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(PowerButt, INPUT);
  pinMode(IncButt, INPUT);
  pinMode(DecButt, INPUT);
 
  Serial.begin(115200);
  Serial.flush();

}

void loop() { // ********************************************************************************
  bouncerON.update ( );
  int valuePow = bouncerON.read(); // read the power button
 
  if (valuePow == LOW) { // button is pressed (low because of PULLLUP resistor)
    if (MotorRun == 0 && bouncerON.fallingEdge() == true) { // motor is off
     
      MotorRun = 1; // turn on the motor
     
      stepperB.setMaxSpeed(SetMotorSpeed);
      stepperB.setSpeed(SetMotorSpeed);
      stepperB.setAcceleration(100);
      stepperB.moveTo(1000000);
      while(stepperB.currentPosition() != 50000){
         stepperB.run(); }
      stepperB.setCurrentPosition(0);
      stepperB.moveTo(1000000);
      stepperB.setSpeed(SetMotorSpeed);
     
    }
   
    else if(MotorRun == 1 && bouncerON.fallingEdge() == true) { // motor is off
      MotorRun = 0; // turn off the motor
      stepperB.setCurrentPosition(0);
      stepperB.setAcceleration(100);
      stepperB.moveTo(60000);
      stepperB.setSpeed(SetMotorSpeed);
    }
  }

stepperB.run();

count++;

//Serial.print(count); Serial.print(" "); Serial.print(MotorRun); Serial.print(" "); Serial.println(stepperB.distanceToGo());

} // END
