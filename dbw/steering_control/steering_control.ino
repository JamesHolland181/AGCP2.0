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

#include <mcp_can.h>
#include <SPI.h>

#include <digitalWriteFast.h>

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
// Steering Variables
//
////////

#define left_encoder A0
#define right_encoder A1
#define pulse 6
#define dir 5

String request = "";

// function prototypes
int can_msg_to_input(void);
void input_handler(String input);

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
}

void loop(){
    // handle receiving user inputs        
    while (Serial.available() > 0){
        char c = Serial.read();
        if(c != '\n'){
          request += c;
        }
        else{Serial.println(request);}
        delay(5);
    }

    
}
