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
#include "mcp2515_can.h"
#include "mcp2515_can_dfs.h"
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
unsigned char broadcast[1] = {0};
int msg=0;

mcp2515_can CAN(SPI_CS_PIN);                                    // Set CS pin

////////
//
// Accelerator Variables
//
////////
#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 Motor;

const int id = 1;   // CAN node ID
int tps = 0;
long voltage = 0;        // PWM signal for D6
double Pedal = analogRead(A0); // pedal position --> if applied, kills RC mode
int gear_selected=1; // 1 for forward, 2 for neutral, 3 for reverse
char current_dir = 'F'; // 'F' for forward, 'N' for neutral, 'R' for reverse
String request = "";

// function prototypes
int can_msg_to_input(void);
void input_handler(String input);

void setup(){
    Motor.begin(0x62);  
    // define analog 0 as an input to measure signal from pedal
    pinMode(Pedal, INPUT);
    // CAN-BUS Setup
    SERIAL.begin(115200);
    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}


void loop(){
    Pedal = analogRead(A0)* (5.0 / 1023.0); //convert signal from pedal to voltage --> when voltage is > 0.10, RC disengaged    
    
    if(Pedal > 0.00){
      Serial.println("Pedal: "+String(Pedal));
    }
    
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 2) {         // if the message is from the gear selector ...
        CAN.readMsgBuf(&len, input); // read can message to update direction

        // determine current direction
        if(input[1] == '1'){current_dir = 'F'; gear_selected = 1;}
        if(input[1] == '2'){current_dir = 'N'; gear_selected = 2;}
        if(input[1] == '3'){current_dir = 'R'; gear_selected = 3;}
        ////Serial.println("Current Direction: "+current_dir);
      }    
      else if(current_dir != 'N'){ // if 'transmission' is in gear
        voltage = input_handler(input, current_dir); // convert can message to input
      }            
//      if(CAN.getCanId()){
//        CAN.readMsgBuf(&len,input);
//        int z=0;
//        while(z<=2){
//          Serial.print(input[z]);
//          z++;
//        }
//        Serial.println(" ");
//      }
    }    

    // handle receiving user inputs        
    while (Serial.available() > 0){
        //Serial.println("reading");
        char c = Serial.read();
        if(c != '\n'){
          request += c;
        }
        delay(5);
    }

    if(request.toInt() > 0){
        tps = request.toInt();
        Serial.print("Throttle Position: ");
        Serial.println(tps);
        voltage = input_handler(tps,current_dir); // convert can message to input
    }
    else if(request.indexOf("S")>=0){
      tps = 0;
      Serial.println("STOP");
      voltage = input_handler(tps,'N');
    }

    // formatting CAN broadcast
    // push broadcast (id, ext, length, buffer)
    broadcast[0] = tps;
    CAN.sendMsgBuf(id, 0, 1, broadcast);
    
    for(int i=0;i<sizeof(broadcast);i++){   
        //Serial.print(broadcast[i]);
    }
    //Serial.println(" ");

    delay(3000);
    request = ""; // reset input field
}


 ////////////////////
// Helper functions //
 ////////////////////

// input handler
int input_handler(int tps, char current_dir){
    voltage = map(tps,0,100,0,4095); // translate from percentage to hex for DAC --> cannot go all the way to 5V, limit is ~4.5V
    // Modulate PWM according to input  
//    Serial.println(voltage);
    if(current_dir == 'R')
    {
        Motor.setVoltage(voltage-5, false); // in reverse, limit the allowed speed
        //analogWrite(Motor,voltage-5); // in reverse, limit the allowed speed
        //Serial.println("Current Dir: Reverse");
        //Serial.println("TPS: "+String(tps));        
        //Serial.println("PWM Signal: "+String(voltage));
    }
    else if(current_dir == 'F')
    {
        Motor.setVoltage(voltage, false);
        //analogWrite(Motor,voltage);
        //Serial.println("Current Dir: Forward");
        //Serial.println("TPS: "+String(tps));        
        //Serial.println("PWM Signal: "+String(voltage));
    }  
    else{
      voltage = 0;
      Motor.setVoltage(voltage, false); // kill power
    }

    return voltage;
}
