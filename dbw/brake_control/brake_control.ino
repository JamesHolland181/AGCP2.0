/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: The microcontroller applies or releases braking force based on the received input.
/*
/* inputs: '0' or '1'
/* Outputs: Apply brakes or Release brakes
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
unsigned char broadcast[3] = {0, 0, 0};//, 0, 0, 0, 0, 0};
int msg=0;

mcp2515_can CAN(SPI_CS_PIN);                                    // Set CS pin

////////
//
// Brake Variables
//
////////
const int id = 3;

unsigned long clk = millis();
int engage[2] = {5,6}; // pin combination for engaging the brake --> IN2, IN1

int speed_control = 7; // for manipulating speed via PWM on H-bridge
int pot_pin = analogRead(A0); // for feedback
int prev_reading = 0;
int maxAnalogReading; // max val of pot 
int minAnalogReading; // min val of pot

String request = ""; // user input --> [ 'braking_dir', 'braking_percentage', 'braking_speed' ]
int braking_dir = 0; // '0' for disengage and '1' for engage
double braking_percentage = 0.0; // desired braking amount
double braking_speed = 0.0;

// function prototypes
int can_msg_to_input(void);
void input_handler(int braking_dir, double braking_percentage, double braking_speed);
int moveToLimit(int Direction);

void setup(){
    // CAN-BUS Setup
    SERIAL.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        //Serial.println("CAN BUS Shield init fail");
        //Serial.println("Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
      
    pinMode(engage[0],OUTPUT);
    pinMode(engage[1],OUTPUT);
    pinMode(speed_control,OUTPUT);
    pinMode(A0, INPUT);

    // find limits of potentiometer
    //maxAnalogReading = moveToLimit(1);
    //minAnalogReading = moveToLimit(-1);    
}

void loop(){    
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId()==9) {         // if CAN is from on-board computer ...
        CAN.readMsgBuf(&len, input); // read can message to update direction
        
        braking_dir = input[1];
        braking_percentage = input[2]; 
        braking_speed = input[3]; 

        input_handler(braking_dir, braking_percentage, braking_speed);
      }   
//      if(CAN.getCanId()){
//        CAN.readMsgBuf(&len,input);
//        int z=0;
//        while(z<=1){
//          Serial.print(input[z]);
//          z++;
//        }
//        Serial.println(" ");
//      }
    }
    // handle receiving user inputs        
    while (Serial.available() > 0){
        ////Serial.printtln("reading");
        char c = Serial.read();
        if(c != '\n'){
          request += c;
        }
        delay(5);
    }  
    if(request == "engage"){
//        Serial.println("Engage 1 ");
        input_handler(1,100,100);
        braking_dir = 1;
        braking_percentage = 100;
        braking_speed = 100;
    }   
    else if(request == "disengage"){
//      Serial.println("Disengage 1 ");
      input_handler(0,100,100);
      braking_dir = 0;
      braking_percentage = 100;
      braking_speed = 100;
    }
    request = "";
    // broadcast to CAN
    broadcast[0] = braking_dir;    broadcast[1] = braking_percentage;    broadcast[2] = braking_speed;   
    
    // push broadcast (id, ext, length, buffer)
    CAN.sendMsgBuf(id, 0, 3, broadcast);
    
    for(int i=0;i<sizeof(broadcast);i++){   
        //Serial.printt(broadcast[i]);
    }
    //Serial.printtln(" ");

    delay(3000);    
}

 ////////////////////
// Helper functions //
 ////////////////////

// input handler
void input_handler(int braking_dir, double braking_percentage, double braking_speed){
  prev_reading=0;
  if(braking_percentage > 1){
    if(braking_dir ==1){
      ////Serial.println("engage 2 ");
      digitalWrite(engage[0],HIGH);
      digitalWrite(engage[1],LOW);
    }
    else if(braking_dir == 0){
      ////Serial.println("disengage 2 ");
      digitalWrite(engage[1],HIGH);
      digitalWrite(engage[0],LOW);
    }
  }    
    // power down
    analogWrite(speed_control,0);
}

// for calibrating linear actuator
int moveToLimit(int Direction){
  int prevReading=0;
  int pot_pin=0;
  do{
    prevReading = pot_pin;
    
    if(Direction ==1){
        digitalWrite(engage[0],HIGH);
        digitalWrite(engage[1],LOW);
    }
    else if(Direction == -1){
        digitalWrite(engage[1],HIGH);
        digitalWrite(engage[0],LOW);
    }

    delay(200); //keep moving until analog reading remains the same for 200ms
    pot_pin = analogRead(A0);
  }while(prevReading != pot_pin);
  return pot_pin;
}
