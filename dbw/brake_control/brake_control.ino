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
    
    if(input.substring(0,input.indexOf(":")) == "B")
    {
        Serial.println("Brake");
        input.remove(0,(input.indexOf(":")+1));
        Serial.println(input);
        
        // Apply brakes
        if(input.toInt() == 1)
        {
        delay(500);
        Serial.println("on");
        digitalWrite(BrakeOff,LOW);
        delay(500);
        digitalWrite(BrakeOn,HIGH);
        }
        
        // Release brakes
        else
        {
        delay(500);
        Serial.println("off");
        digitalWrite(BrakeOn,LOW);
        delay(500);
        digitalWrite(BrakeOff,HIGH);
        }    

        delay(2000);
        digitalWrite(BrakeOn,LOW);
        digitalWrite(BrakeOff,LOW);
    }    
    
    input = "";
}
