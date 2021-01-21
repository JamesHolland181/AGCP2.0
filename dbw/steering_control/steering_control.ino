/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: Microcontroller manipulates a stepper motor attached to the steering column of golf cart based on received
/* input. Sketch calculates the necessary rotations of stepper to steer in desired direction and .
/*
/* Inputs: <>
/* Outputs: <>
/* Example: 
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
unsigned char broadcast[1] = {0};//, 0, 0, 0, 0, 0, 0, 0};
int msg=0;

mcp2515_can CAN(SPI_CS_PIN);                                    // Set CS pin

////////
//
// Steering Variables
//
////////

#define left_encoder A0
#define right_encoder A1
#define pulse 6
#define dir 5

String request = "";
bool rot_dir = 1; // '0' for counter clockwise and '1' for clockwise
int rot_speed = 0; // 0-100% RPM in direction of rotation previously defined

// function prototypes
int can_msg_to_input(void);
void input_handler(bool rot_dir, int rot_speed);

void setup(){
    // Stepper Setup
  
    // CAN-BUS Setup
    SERIAL.begin(115200);
    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        ////Serial.println("CAN BUS Shield init fail");
        //Serial.println("Init CAN BUS Shield again");
        delay(100);
    }
    //Serial.println("CAN BUS Shield init ok!");

    pinMode(pulse,OUTPUT);
    pinMode(dir,OUTPUT);
    pinMode(left_encoder,INPUT);
    pinMode(right_encoder,INPUT);    
}

void loop(){
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 9) {         // if the message is from the SpeedGoat ...
        CAN.readMsgBuf(&len, input); // read can message to update direction
        int z=0;
        
        rot_dir = input[0];
        rot_speed = input[1];
        
        while(z < sizeof(input)){
          Serial.print(input[z]);
          z++;
        }
        Serial.println(" ");
        input_handler(rot_dir,rot_speed);
      }
    }    
  
    // handle receiving user inputs --> simply direction or speed at the moment
    while (Serial.available() > 0){
        char c = Serial.read();
        if(c != '\n'){
          request += c;
        }
        else{Serial.println(request);}
        delay(5);
    }
    
    // Parse input --> Forward and Reverse
    if(request=="forward"){
      Serial.println(request);
        digitalWrite(dir,HIGH);
    }
    else if(request == "reverse"){
      Serial.println(request);
        digitalWrite(dir,LOW);
    }
        
    digitalWrite(pulse,HIGH);
    delay(1); // Test without this as well
    digitalWrite(pulse,LOW);        
    
    for(int i=0;i<sizeof(broadcast);i++){   
        //Serial.print(broadcast[i]);
    }
    //Serial.println(" ");

    delay(3000);
    
    request = "";
}
