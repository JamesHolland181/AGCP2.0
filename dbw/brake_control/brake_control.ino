/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: The microcontroller applies or releases braking force based on the received input.
/*
/* Inputs: '0' or '1'
/* Outputs: Apply brakes or Release brakes
/*********************************************************************************************************************/

// Brake Variables

int engage[2] = {3,5}; // pin combination for engaging the brake --> IN1 HIGH, IN2 LOW
int disengage[2] = {3,5}; // pin combination for disengaging the brake --> IN1 LOW, IN2 LOW
int speed_control = 6; // control speed of actuator with PWM from D6
int braking_percentage = 0;
int pwm = 0; // 0-255, control speed of actuator/braking

int potentiometer = A0; // for feedback
int maxAnalogReading; // max val of pot
int minAnalogReading; // min val of pot
bool limit = false;
String input = "";

void setup(){
 
    Serial.begin(9600);    

    pinMode(engage[0],OUTPUT);
    pinMode(engage[1],OUTPUT);

    pinMode(disengage[0],OUTPUT);
    pinMode(disengage[1],OUTPUT);

    pinMode(speed_control,OUTPUT);

    pinMode(potentiometer, INPUT);

    // find limits of potentiometer
    maxAnalogReading = moveToLimit(1);
    minAnalogReading = moveToLimit(-1);
}

void loop(){
    // handle receiving inputs
    while (Serial.available() > 0){
        char c = Serial.read();
        input += c;
        delay(5);
    }
    
    if(input != ""){
        Serial.print("MSG: ");
        Serial.println(input);
    }
    
    if(input.substring(0,input.indexOf(":")) == "B")
    {
        Serial.println("Brake");
        input.remove(0,(input.indexOf(":")+1));
        Serial.println(input);
        pwm = map(input.toInt(),0,100,40,255); 

        // Apply brakes
        if(input.toInt() >= 1)
        {
        delay(500);
        Serial.println("on");
        
        // implement feedback
        while(limit == false){
          digitalWrite(speed_control,pwm);
          digitalWrite(engage[0],HIGH);

          // break from loop if limits have been reached
          if((A0 >= maxAnalogReading)or(A0 <= minAnalogReading)){ break;}

          // break from loop if desired braking percentage reached
          if((A0/(maxAnalogReading-minAnalogReading)) == braking_percentage){break;}
        }

        // power down
        digitalWrite(speed_control,0);
        digitalWrite(engage[0],LOW); 
        digitalWrite(engage[1],LOW); 
        
        }
        
        // Release brakes
        else
        {
        delay(500);
        Serial.println("off");
         
        while(limit == false){
          digitalWrite(speed_control,pwm);
          digitalWrite(disengage[0],HIGH);

          if((A0 >= maxAnalogReading)or(A0 <= minAnalogReading)){ break;}
        }

        // power down 
        digitalWrite(speed_control, 0);
        digitalWrite(disengage[0],LOW); 
        digitalWrite(disengage[1],LOW);
        }    

        // implement potentiometer feedback to stop power supply
        delay(2000);
        digitalWrite(disengage[0],LOW);
        digitalWrite(disengage[1],LOW);
    }    
    
    input = "";
}

int moveToLimit(int Direction){
  int prevReading=0;
  int currReading=0;
  do{
    prevReading = currReading;
    
    if(Direction ==1){
        digitalWrite(speed_control,pwm);
        digitalWrite(engage[0],HIGH);
        digitalWrite(engage[1],LOW);
    }
    else if(Direction == -1){
        digitalWrite(speed_control,pwm);
        digitalWrite(disengage[0],HIGH);
        digitalWrite(disengage[1],LOW);
    }

    delay(200); //keep moving until analog reading remains the same for 200ms
    currReading = analogRead(potentiometer);
  }while(prevReading != currReading);
  return currReading;
}
