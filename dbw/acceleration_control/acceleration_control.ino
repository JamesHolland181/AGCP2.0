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

int Motor = 2;
int Pedal = analogRead(A0);
int pwm = 0;
char current_dir='F';
const int id = 1;

 ////////////////////
// Helper functions //
 ////////////////////

// Receive CAN messages
int can_msg_to_input(void){
    CAN.readMsgBuf(&len, input);    // read data,  len: data length, buf: data buf

    unsigned long canId = CAN.getCanId();

    SERIAL.println("-----------------------------");
    SERIAL.print("Get data from ID: ");
    SERIAL.println(canId);

    // print the data
    for (int i = 0; i < len; i++) { // print the data
        SERIAL.print(input[i]);
        SERIAL.print("\t");
    }
    SERIAL.println();
    return input; // append to input
}

// input handler
void input_handler(int input){
    // if input is an 'S' --> kill power to motor
    if(input == 0)
    {
        analogWrite(Motor,0);
    }    
    
    // Modulate PWM according to input  
    if(current_dir == 'R')
    {
        pwm = map(input,0,100,40,255)-5; // in reverse, limit the allowed speed
        analogWrite(Motor,pwm);
        Serial.println("Current Dir: Reverse");
        Serial.println("TPS: "+input);        
        Serial.println("PWM Signal: "+pwm);
    }
    else if(current_dir == "F")
    {
        pwm = map(input,0,100,40,255); 
        analogWrite(Motor,pwm);
        Serial.println("Current Dir: Forward");
        Serial.println("TPS: "+input);        
        Serial.println("PWM Signal: "+pwm);
    }  
    else{
      analogWrite(Motor,0); // kill signal
    }
}

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

unsigned char old_broadcast[8];

void loop(){
//    broadcast[5] = 0; broadcast[6] = 0; broadcast[7] = 0; // reset CAN broadcast

//    old_broadcast = broadcast; // holds last broadcast to ensure redundant information is being sent out
  
    Pedal = analogRead(A0);
    
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 2) {         // if the message is from the gear selector ...
        CAN.readMsgBuf(&len, input); // read can message to update direction
        current_dir = input;
        Serial.println("Current Direction: "+current_dir);
      }    
      else
      {         
        input_handler(can_msg_to_input()); // convert can message to input
      }            
    }    
    
    String input="";
    while (Serial.available()>0) {         // check if data coming
        input+=Serial.read();
        delay(5);
    }
     if(input != ""){ 
       input_handler(100); // convert can message to input
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
