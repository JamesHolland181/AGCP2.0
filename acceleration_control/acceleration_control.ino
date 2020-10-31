/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: Microcontroller manipulates a PWM signal to the DC motor controller to regulate applied power. This system
/* manipulates the signal that would normally be altered by an inductive potentiometer in the pedal. The DC motor 
/* controller references the signal from this system as well as the gear selector to apply power.
/*
/* Inputs: <percentage of power>
/* Outputs: <proportionate power>
/* Example: '100' --> Full power 
/*********************************************************************************************************************/

int Motor = 2;
int Pedal = analogRead(A0);

void setup(){
    // define the function of the Motor pin
    pinMode(Motor, OUTPUT);
    Serial.begin(9600);
}

void loop(){
    Pedal = analogRead(A0);
    
    // handle receiving inputs
    while (Serial.available() > 0){
        char c = Serial.read();
        input = += c;
        delay(5);
    }
    
    if(input != ""){
        Serial.print("MSG: ");
        Serial.println(input);
    }
    
    // if input is an 'S' --> kill power to motor
    if(input.substring(0,1) == "S")
    {
        analogWrite(Motor,0);
        digitalWrite(DirectionRelay,LOW);
    }    
    
    // Modulate PWM according to input  
    if(input.substring(0,input.indexOf(":")) == "R")
    {
        digitalWrite(DirectionRelay,HIGH);
        Serial.println("reverse");
        input.remove(0,(input.indexOf(":")+1));  
        Serial.println(input);
        int pwm = map(input.toInt(),0,100,40,255);
        analogWrite(Motor,pwm);
        Serial.println(pwm);
        
    }
    else if(input.substring(0,input.indexOf(":")) == "F")
    {
        digitalWrite(DirectionRelay,LOW);
        Serial.println("forward");
        input.remove(0,(input.indexOf(":")+1));
        Serial.println(input);
        int pwm = map(input.toInt(),0,100,40,255);
        analogWrite(Motor,pwm);
        Serial.println(pwm);
        
    }    
    input = "";
}
