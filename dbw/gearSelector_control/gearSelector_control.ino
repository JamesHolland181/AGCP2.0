/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 10/26/2020
/* Note: Adapted from code written by Bruce Hicks during the July 2020 AGCP effort at Florida Polytechnic University.
/* 
/* Purpose: Microcontroller engages the desired gear, forward or reverse, depending on the received input. This system
/* controls the direction relay that the DC motor controller refences for applying power.
/*
/* Inputs: 'F', 'R', 'N'
/* Outputs: Forward, Reverse, Neutral
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
unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char broadcast[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int msg=0;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

////////
//
// Gear Selector Variables
//
////////

int DirectionRelay = 3;
int ForwardSwitch = 5;
int ReverseSwitch = 4;
int current_dir = 000;

 ////////////////////
// Helper functions //
 ////////////////////

// Receive CAN messages
char can_msg_to_input(void){
    len = 0;

    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

    unsigned long canId = CAN.getCanId();

    SERIAL.println("-----------------------------");
    SERIAL.print("Get data from ID: 0x");
    SERIAL.println(canId, HEX);
    
    for (int i = 0; i < len; i++) { // print the data
        SERIAL.print(buf[i], HEX);
        SERIAL.print("\t");
    }
    SERIAL.println();
    
    return buf; // append to input
}

// input handler
void input_handler(char input){  
    int F_Direction = digitalRead(ForwardSwitch);
    int R_Direction = digitalRead(ReverseSwitch);
    
    while(F_Direction == HIGH || R_Direction == HIGH)
    {
        int F_Direction = digitalRead(ForwardSwitch);
        int R_Direction = digitalRead(ReverseSwitch);
        //Serial.println(F_Direction);
        //Serial.println(R_Direction);
        
        if(F_Direction == HIGH)
            {
                digitalWrite(DirectionRelay, LOW); current_dir = 100;
            }
        else if(R_Direction == HIGH)
            {
                digitalWrite(DirectionRelay,HIGH); current_dir = 001;
            }
        else
            {
                current_dir = 000; break;
            }      
    }
}    

void setup(){
    // define the function of the pins
    pinMode(DirectionRelay, OUTPUT);    
    pinMode(ForwardSwitch, INPUT);
    pinMode(ReverseSwitch, INPUT);
    
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
  //Serial.println("in loop");
    broadcast[5] = 0; broadcast[6] = 0; broadcast[7] = 0; // reset CAN broadcast
  
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
        input_handler(can_msg_to_input()); // convert can message to input  
          
        // formatting CAN broadcast
        if(current_dir = 100){
          broadcast[5] = 1;
        }
        else if(current_dir = 001){
          broadcast[7] = 1;
        }
        else{
          broadcast[5] = 0; broadcast[6] = 0; broadcast[7] = 0;
        }
    }

    // push broadcast (id, ext, length, buffer)
    CAN.sendMsgBuf(0x02, 0, 8, broadcast);
    delay(3000);
}
    
