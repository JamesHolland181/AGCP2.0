#include <AccelStepper.h>

int SetMotorSpeed = 2000;
AccelStepper stepperA(1, 11, 8);
AccelStepper stepperB(1, 3, 4);

void setup()
{ 
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop()
{
stepperB.setCurrentPosition(0);
stepperA.setCurrentPosition(0);
     
stepperB.setMaxSpeed(SetMotorSpeed);
stepperA.setMaxSpeed(SetMotorSpeed);

stepperB.setAcceleration(100);
stepperA.setAcceleration(100);
 
stepperB.moveTo(1000000);
stepperA.moveTo(1000000);
     
  while((stepperB.currentPosition()) != 50000){
   stepperB.run();
   stepperA.run();}

     
    stepperB.stop();
    stepperA.stop();

while (stepperA.run() && stepperB.run()) ;
   
}
