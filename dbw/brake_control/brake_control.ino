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
// Brake Variables
//
////////

int engage[2] = {3,5}; // pin combination for engaging the brake --> IN1 HIGH, IN2 LOW
int disengage[2] = {3,5}; // pin combination for disengaging the brake --> IN1 LOW, IN2 LOW
int speed_control = 6; // control speed of actuator with speed_signal from D6
int speed_signal = 0; // 0-255, control speed of actuator/braking

int potentiometer = A0; // for feedback
int maxAnalogReading; // max val of pot
int minAnalogReading; // min val of pot
bool limit = false; // limit reached?

String user_input = ""; // --> [ 'B:', 'braking_percentage %', 'braking_speed %' ]
int braking_dir = 0; // 1 --> engage; 0 --> disengage
int braking_percentage = 0; // desired braking amount
int braking_speed = 0; // speed of actuator motion


void setup(){
    // CAN-BUS Setup
    SERIAL.begin(115200);
//
//    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
//        SERIAL.println("CAN BUS Shield init fail");
//        SERIAL.println("Init CAN BUS Shield again");
//        delay(100);
//    }
//    SERIAL.println("CAN BUS Shield init ok!");
      
    pinMode(engage[0],OUTPUT);
    pinMode(engage[1],OUTPUT);
    pinMode(disengage[0],OUTPUT);
    pinMode(disengage[1],OUTPUT);
    pinMode(speed_control,OUTPUT);
    pinMode(potentiometer, INPUT);

    // find limits of potentiometer
    maxAnalogReading = moveToLimit(1);
    minAnalogReading = moveToLimit(-1);
}

unsigned char old_broadcast[8];

void loop(){    
      
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 9) {         // if the message is from the gear selector ...
        CAN.readMsgBuf(&len, input); // read can message to update direction

        // parse CAN message into inputs 
        braking_dir = input[0]+input[1];
        braking_percentage = (input[2]+input[3]+input[4]);//.toInt();
        braking_speed = input[5]+input[6]+input[7];
      }             
    }    
    
    while (Serial.available() > 0){
        char c = Serial.read();
        user_input += c;
        delay(5);
    }
    
    if(user_input != ""){
        Serial.print("User input: ");
        Serial.println(user_input);
    }
    
    if(user_input.substring(0,user_input.indexOf(":")) == "B")
    {
        Serial.println("Brake");
        user_input.remove(0,(user_input.indexOf(":")+1));
        Serial.println(user_input);

        
    }    
    
    user_input = "";    
}


 ////////////////////
// Helper functions //
 ////////////////////

// for calibrating linear actuator
int moveToLimit(int Direction){
  int prevReading=0;
  int currReading=0;
  do{
    prevReading = currReading;
    
    if(Direction ==1){
        digitalWrite(speed_control,speed_signal);
        digitalWrite(engage[0],HIGH);
        digitalWrite(engage[1],LOW);
    }
    else if(Direction == -1){
        digitalWrite(speed_control,speed_signal);
        digitalWrite(disengage[0],HIGH);
        digitalWrite(disengage[1],LOW);
    }

    delay(200); //keep moving until analog reading remains the same for 200ms
    currReading = analogRead(potentiometer);
  }while(prevReading != currReading);
  return currReading;
}

// translating inputs into actions
void input_handler(int braking_dir, int braking_percentage, int braking_speed){
  // inputs: 
  // "braking_dir"        --> integer val acting as bool; 1 for engage and 0 for disengage
  // "braking_percentage" --> integer val 0-100
  // "braking_speed"      --> integer val 0-100
  
  speed_signal = map(braking_speed,0,100,40,255); // translate desired speed rate to speed_signal signal

  // Apply brakes
  if(braking_percentage >= 1)
  {
  delay(500);
  Serial.println("engage");
  
      // while the limit hasn't been reached
      while(limit == false){
        digitalWrite(speed_control,speed_signal); // set braking speed
    
        // engage brakes
        digitalWrite(engage[0],HIGH);
        digitalWrite(engage[1],LOW);
    
        // break from loop if limits have been reached OR if desired braking percentage reached
        if(((A0 >= maxAnalogReading)or(A0 <= minAnalogReading))or((A0/(maxAnalogReading-minAnalogReading)) == braking_percentage)){ break;}
      }
  }
  
  // Release brakes
  else
  {
      delay(500);
      Serial.println("disengage");
       
      while(limit == false){
        digitalWrite(speed_control,speed_signal);
        digitalWrite(disengage[0],HIGH);
        digitalWrite(disengage[1],LOW);
    
      // break from loop if limits have been reached OR if desired braking percentage reached
      if(((A0 >= maxAnalogReading)or(A0 <= minAnalogReading))or((A0/(maxAnalogReading-minAnalogReading)) == braking_percentage)){ break;};}
  }  
  
  // power down
  digitalWrite(speed_control,0);
  digitalWrite(engage[0],LOW); 
  digitalWrite(engage[1],LOW); 
  digitalWrite(disengage[0],LOW); 
  digitalWrite(disengage[1],LOW);    
}
