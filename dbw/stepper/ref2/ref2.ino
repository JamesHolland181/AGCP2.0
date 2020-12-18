
/*

Control of a 2-axis gantry using 4 buttons

- all steppers controlled by Big Easy Drivers
- (2) steppers on the X-axis... X2 is mounted in reverse so we run it in reverse
- AccelStepper library makes for smooth accel / decel
- (4) user navigation buttons : up/down, left/right. Code is blocking... we don't care about diagonal moves, and don't have t
   deal with UP / DOWN fighting each other this way.
- limit switches on both axes
- running on a MEGA 2560

*/

#include <AccelStepper.h>

const int MX1_Step = 10;                      // Big Easy Driver pins
const int MX1_Dir = 11;
const int MX2_Step = 7;
const int MX2_Dir = 9;
const int MX_Enable = 22;
const int MY1_Step = 12;
const int MY1_Dir = 13;
const int MY_Enable = 23;

const int navUp = 47;                          // Navigation buttons
const int navDown = 51;
const int navLeft = 53;
const int navRight = 49;

AccelStepper X1(1, MX1_Step, MX1_Dir);         
AccelStepper X2(1, MX2_Step, MX2_Dir);
AccelStepper Y1(1, MY1_Step, MY1_Dir);

const int maxSpeed = 800;
const int accel = 600;
const int xLimit = 6500;                      // Right end of X axis
const int yLimit = 1800;                      // Back/far end of Y axis

/************************************************************************************************************/
void setup(){ 
  Serial.begin(9600);

  pinMode(navUp, INPUT_PULLUP); 
  pinMode(navDown, INPUT_PULLUP);   
  pinMode(navLeft, INPUT_PULLUP);   
  pinMode(navRight, INPUT_PULLUP);
  pinMode(MX_Enable, OUTPUT);
  pinMode(MY_Enable, OUTPUT);
 
  digitalWrite(MX_Enable, HIGH);                  // Active LOW.... disable motors to start
  digitalWrite(MY_Enable, HIGH);                  // Active LOW.... disable motors to start
 
  X1.setMaxSpeed(maxSpeed);
  X2.setMaxSpeed(maxSpeed);
  Y1.setMaxSpeed(maxSpeed);

  X1.setAcceleration(accel);
  X2.setAcceleration(accel);
  Y1.setAcceleration(accel);
 
  X2.setPinsInverted(true, false, false);            // Dir, Step, Enable : X2 mounted reverse, so opposite rotation

  X1.setCurrentPosition(0);
  X2.setCurrentPosition(0);
  Y1.setCurrentPosition(0);

  Serial.println("Setup complete");
}
/************************************************************************************************************/
void loop(){
  if (!digitalRead(navUp)){            // once we see the first button press....
    Y1.moveTo(yLimit);                 // set the Target
    if (Y1.distanceToGo()!=0){            // and if we need to
      moveUp();                        // then move the motor
    }
  } 
  if (!digitalRead(navDown)){
    Y1.moveTo(0);
    if (Y1.distanceToGo()!=0){
      moveDown();
    }
  }
  if (!digitalRead(navRight)){
    X1.moveTo(xLimit);
    X2.moveTo(xLimit);
    moveRight();
  }
  if (!digitalRead(navLeft)){
    X1.moveTo(0);
    X2.moveTo(0);
    moveLeft();
  }
} 

/************************************************************************************************************/
void moveUp() {
  digitalWrite(MY_Enable, LOW);            // Enable the Big Easy Driver motor outputs
  while (!digitalRead(navUp)&&Y1.distanceToGo()!=0){             // Keep looping here as long as the button's held down
    Y1.run();
  }                                        // ... or the button was let go
  Y1.stop();                               // tell AccelStepper to decel to a smooth stop ASAP, if it's not at the Target
  while (Y1.distanceToGo()!=0){             // loop until you get to the new decel target
    Y1.run();
  }
  digitalWrite(MY_Enable, HIGH);            // disable the BED to save motor current (keep motors cool.... no load)
}

void moveDown() {
  digitalWrite(MY_Enable, LOW);
  while (!digitalRead(navDown)&&Y1.distanceToGo()!=0){           
    Y1.run();
  }
  Y1.stop();   
  while (Y1.distanceToGo()!=0){           
    Y1.run();
  } 
  digitalWrite(MY_Enable, HIGH);           
}

void moveLeft() {
  digitalWrite(MX_Enable, LOW);           
  while (!digitalRead(navLeft)&&X1.distanceToGo()!=0){           
    X1.run();                              // Remember we have 2 motors to run on the X axis
    X2.run();
  } 
  X1.stop();                               
  X2.stop();
  while (X1.distanceToGo()!=0){             
    X1.run();
    X2.run();
  }
digitalWrite(MX_Enable, HIGH);           
}

void moveRight() {
  digitalWrite(MX_Enable, LOW);           
  while (!digitalRead(navRight)&&X1.distanceToGo()!=0){         
    X1.run();
    X2.run();
  } 
  X1.stop();                               
  X2.stop();
  while (X1.distanceToGo()!=0){             
    X1.run();
    X2.run();
  }
digitalWrite(MX_Enable, HIGH);           
}
