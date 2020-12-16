#include <AccelStepper.h>
#include <digitalWriteFast.h>

//#define left_encoder A0
//#define right_encoder A1
//#define pulse 6
//#define dir 5

// Define a stepper and the pins it will use
AccelStepper stepper(uint8_t interface = AccelStepper::DRIVER,
    uint8_t   dir = 5,
    uint8_t   pulse = 6,
    bool    enable = true 
  ); // AccelStepper DRIVER object, two driver pins --> 'dir': direction, 'pulse': for triggering step

void setup()
{  
  // Setting initial parameters of steering --> consider having these parameters change depending on vehicle speed (sim-ware should be providing this as an output)
  stepper.setMaxSpeed(100);
  stepper.setAcceleration(20);

  // Need?
  //pinMode(6,OUTPUT);
  //pinMode(5,OUTPUT);

  // Experimental --> uses "digitalWriteFast" library for faster timing
  //pinModeFast(left_encoder,INPUT);
  //pinModeFast(right_encoder,INPUT);
  //pinModeFast(pulse,OUTPUT);
  //pinModeFast(dir,OUTPUT);  
  pinMode(left_encoder,INPUT);
  pinMode(right_encoder,INPUT);
}

void loop() // --> I suggest that we use encoders on the physical wheels to monitor position and simply power on the motor until it reaches the desired wheel angle (may have to use regular encoders and do a calibration sequence)
{           // --> Another option is to use an IMU and power the motor until the cart reaches the desired heading
    // handle receiving user inputs --> simply direction or speed at the moment
    while (Serial.available() > 0){
        char c = Serial.read();
        if(c != '\n'){
          request += c;
        }
        else{Serial.println(request);}
        delay(5);
    }
    if(request.toInt() > 0){
        // inputhandler(); --> to-do
        request = ""; // reset input field
    }
    // Parse input --> 1 for right, -1 for left
    if(strstr(request,"Speed")==1){
      stepper.setMaxSpeed(99);
    }
    else if(strstr(request,"For")==1){
        stepper.move(10);
        digitalWrite(dir,HIGH);
        stepper.run();// comment out if running manually
    }
    else if(strstr(request,"Rev")==1){
        stepper.move(-10)
        digitalWrite(dir,LOW);
        stepper.run();// comment out if running manually
    }
    
    //uncomment if running manually
    /*
     * for(int x=0;x<20;i++){
     *    digitalWriteFast(pulse,HIGH);
     *    delay(1); // Test without this as well
     *    digitalWriteFast(pulse,LOW);
     * }
     */
        
    stepper.run(); // comment out if running manually
}
