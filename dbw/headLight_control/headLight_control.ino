/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: Microcontroller power on digital pins to manipulate on-baord headlight harness. The program is capable of
/* turning headlights and high beams on and off as well as flashing them.
/*
/* Inputs: 
/* Outputs: none
/*********************************************************************************************************************/

// NEEDS REVISION


void setup(){
    
    
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
    
    // turn on headlights
    
    // turn off headlights
    
    // flash headlights
    
    // turn on brights
    
    // turn off brights
    
    // flash brights
    
    input = "";
}
