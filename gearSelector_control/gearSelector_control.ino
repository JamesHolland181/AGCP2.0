/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: Microcontroller engages the desired gear, forward or reverse, depending on the received input. This system
/* controls the direction relay that the DC motor controller refences for applying power.
/*
/* Inputs: 'F', 'R', 'N'
/* Outputs: Forward, Reverse, Neutral
/*********************************************************************************************************************/


// NEEDS REVISION

int DirectionRelay = 3;
int ForwardSwitch = 5;
int ReverseSwitch = 4;

void setup(){
    // define the function of the pins
    pinMode(DirectionRelay, OUTPUT);    
    pinMode(ForwardSwitch, INPUT);
    pinMode(ReverseSwitch, INPUT);
    
    Serial.begin(9600);    
}


// Inside this loop, blah blah blah shall occur
void loop(){
    int F_Direction = digitalRead(ForwardSwitch);
    int R_Direction = digitalRead(ReverseSwitch);
    
    
    while(F_Direction == HIGH || R_Direction == HIGH)
    {
        int F_Direction = digitalRead(ForwardSwitch);
        int R_Direction = digitalRead(ReverseSwitch);
        //Serial.println(F_Direction);
        //Serial.println(R_Direction);
        
        if(F_Direction == HIGH)
            {
                digitalWrite(DirectionRelay, LOW);
            }
        else if(R_Direction == HIGH)
            {
                digitalWrite(DirectionRelay,HIGH);
            }
        else
            {
                    analogWrite(Motor,0);
                    digitalWrite(DirectionRelay,LOW);
                break;
            }
            Serial.println(Pedal);
            Pedal = analogRead(A0);
        
        if(Pedal < 80)
            {
                analogWrite(Motor,0);
            }
        else
            {
            int pwm = map(Pedal,80,885,0,255);
            
            if(R_Direction == HIGH)
                {
                    analogWrite(Motor,pwm-5); 
                }
            else
                {
                    analogWrite(Motor,pwm); 
                }
            }
        
    }    
    
    input = "";
}
    
