/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: Microcontroller manipulates power on pins in order to control turn signals based on input. The system can
 * indicate left, right, and both in a hazard state.
/*
/* Inputs: LL, RL, FL, SL
/* Outputs: Left, Right, Hazard, Off
/*********************************************************************************************************************/


// NEEDS REVISION

int LeftTurnSig = 6;
int RightTurnSig = 7;

void setup(){
    //set the function of the pins
    pinMode(LeftTurnSig,OUTPUT);
    pinMode(RightTurnSig,OUTPUT);    

    digitalWrite(LeftTurnSig, HIGH);
    digitalWrite(RightTurnSig, HIGH);

    Serial.begin(9600);       
}

void loop(){
    
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
    
    // left turn signal
    if(input.substring(0,2) == "LL")
    {
        Serial.println("Left");
        digitalWrite(LeftTurnSig,LOW);
        digitalWrite(RightTurnSig,HIGH);
    }
    
    // right turn signal
    else if(input.substring(0,2) == "RL")
    {
        Serial.println("Right");
        digitalWrite(RightTurnSig,LOW);
        digitalWrite(LeftTurnSig,HIGH);
    }
    
    // hazard signal
    else if(input.substring(0,2) == "FL")
    {
        Serial.println("Flash");
        digitalWrite(RightTurnSig,LOW);
        digitalWrite(LeftTurnSig,LOW);
    }
    
    // signal off
    else if(input.substring(0,2) == "SL")
    {
        Serial.println("Flash off");
        digitalWrite(RightTurnSig,HIGH);
        digitalWrite(LeftTurnSig,HIGH);
    }
    
    input = "";
}
