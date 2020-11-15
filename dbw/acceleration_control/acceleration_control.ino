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
///////
//
// CAN-Bus Preamble
// 
///////

#include <mcp_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

unsigned char len = 0;
unsigned char input[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char broadcast[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int msg=0;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

////////
//
// Accelerator Variables
//
////////

const int id = 1;   // CAN node ID
int Motor = 2;      // D2 --> for sending PWM to DC motor controller
int pwm = 0;        // PWM signal for D2
int Pedal = analogRead(A0); // pedal position --> if applied, kills autonomous mode
char gear_selected[3]="010"; // "100" for forward, "010" for neutral, "001" for reverse
char current_dir = 'N'; // 'F' for forward, 'N' for neutral, 'R' for reverse
char request[3] = "";

// function prototypes
int can_msg_to_input(void);
void input_handler(String input);

void setup(){
    // define the function of the Motor pin
    pinMode(Motor, OUTPUT);

    // CAN-BUS Setup
    SERIAL.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println("Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");
}


void loop(){
    Pedal = analogRead(A0);
    
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 2) {         // if the message is from the gear selector ...
        CAN.readMsgBuf(&len, input); // read can message to update direction

        // determine current direction
        if(input[5] == '1'){current_dir = 'F'; gear_selected[3] = "100";}
        if(input[6] == '1'){current_dir = 'N'; gear_selected[3] = "010";}
        if(input[7] == '1'){current_dir = 'R'; gear_selected[3] = "001";}
        Serial.println("Current Direction: "+current_dir);
      }    
      else if(current_dir != 'N'){ // if 'transmission' is in gear
        input_handler(input, current_dir); // convert can message to input
      }            
    }    

    // handle receiving user inputs        
    while (Serial.available() > 0){
        char c = Serial.read();
        strcat(request,c);
        delay(5);
    }
    
    if(request != ""){
        Serial.print("MSG: ");
        Serial.println(request);
        input_handler(request,current_dir); // convert can message to input
        request[0] = '0'; // reset input field
    }

    // formatting CAN broadcast
    if(pwm < 100){
      broadcast[6] = pwm/10; //tens
      broadcast[7] = pwm%10; // ones
    }
    else{
      broadcast[5] = pwm/100; // hundreds
      broadcast[6] = pwm/10; //tens
      broadcast[7] = pwm%10; // ones
    }
    
    // push broadcast (id, ext, length, buffer)
    CAN.sendMsgBuf(id, 0, 8, broadcast);
    delay(3000);
}


 ////////////////////
// Helper functions //
 ////////////////////

// input handler
void input_handler(String tps, char current_dir){
    int pwm = map(tps.toInt(),0,100,40,255); // translate from percentage to PWM
    
    if(pwm == 0)
    {
        analogWrite(Motor,0);
    }    
    
    // Modulate PWM according to input  
    if(current_dir == 'R')
    {
        analogWrite(Motor,pwm-5); // in reverse, limit the allowed speed
        Serial.println("Current Dir: Reverse");
        Serial.println("TPS: "+tps);        
        Serial.println("PWM Signal: "+pwm);
    }
    else if(current_dir == "F")
    {
        analogWrite(Motor,pwm);
        Serial.println("Current Dir: Forward");
        Serial.println("TPS: "+tps);        
        Serial.println("PWM Signal: "+pwm);
    }  
    else{
      analogWrite(Motor,0); // kill signal
    }
}
