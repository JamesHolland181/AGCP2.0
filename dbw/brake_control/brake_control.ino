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

int current_pos = analogRead(A0); // for feedback
int maxAnalogReading; // max val of pot
int minAnalogReading; // min val of pot

char request[9] = ""; // user input --> [ 'braking_dir', 'braking_percentage', 'braking_speed' ]
int braking_dir = 0; // '0' for disengage and '1' for engage
int braking_percentage = 0; // desired braking amount
int braking_speed = 0; // speed of actuator motion

// function prototypes
int can_msg_to_input(void);
void input_handler(int braking_dir, int braking_percentage, int braking_speed);
int moveToLimit(int Direction);

void setup(){
    // CAN-BUS Setup
    SERIAL.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println("Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");
      
    pinMode(engage[0],OUTPUT);
    pinMode(engage[1],OUTPUT);
    pinMode(disengage[0],OUTPUT);
    pinMode(disengage[1],OUTPUT);
    pinMode(speed_control,OUTPUT);
    pinMode(A0, INPUT);

    // find limits of potentiometer
    maxAnalogReading = moveToLimit(1);
    minAnalogReading = moveToLimit(-1);
}


void loop(){    
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 9) {         // if CAN is from on-board computer ...
        CAN.readMsgBuf(&len, input); // read can message to update direction
        
        braking_dir = input[1];
        braking_percentage = input[2]+input[3]+input[4]; 
        braking_speed = input[5]+input[6]+input[7]; 
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
        braking_dir = strtok(request,':');
        braking_percentage = strtok(request,':');
        braking_speed = strtok(request,':');
        input_handler(braking_dir, braking_percentage, braking_speed); // convert can message to input
        request[0] = '0'; // reset request field
    }   
}


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
void input_handler(int braking_dir, int braking_percentage, int braking_speed){
    // if braking_dir is '1' --> engage brakes by powering on 'engage[0]' (D3)
    if(braking_dir == 1)
    {
          // Modulate speed_signal according to input  
          if(braking_percentage >= 1)
          {
          Serial.println("on");
          
          // potentiometer feedback
          while(true){
            // update current position
            current_pos = analogRead(A0);
            // manipulate speed with pwm signal on pin D6 to 'ENA' on H-bridge
            digitalWrite(speed_control,map(braking_speed,0,100,0,255));    // convert braking speed to pwm signal
            // rotate via HIGH on 'D3' to 'IN1' on H-bridge
            digitalWrite(engage[0],HIGH);
      
            // break from loop if desired position has been reached
            if((current_pos/(maxAnalogReading-minAnalogReading) == braking_percentage)){ break;}
          }
          }
    }
    // if braking_dir is '0' --> disengage brakes by powering on 'disengage[0]' (D5)
    else
    {
        Serial.println("off");
        
        // Modulate speed_signal according to input  
        if(braking_percentage >= 1){
            while(true){
              // update current position
              current_pos = analogRead(A0);              
              // manipulate speed with pwm signal on pin D6 to 'ENA' on H-bridge
              digitalWrite(speed_control,map(braking_speed,0,100,0,255));    // convert braking speed to pwm signal
              // rotate via HIGH on 'D5' to 'IN2' on H-bridge
              digitalWrite(disengage[0],HIGH);
      
            // break from loop if desired position has been reached
            if((current_pos/(maxAnalogReading-minAnalogReading) == braking_percentage)){ break;}
            }
        }
    }     
    // power down
    digitalWrite(speed_control,0);
    digitalWrite(engage[0],LOW); 
    digitalWrite(engage[1],LOW); 
}

// for calibrating linear actuator
int moveToLimit(int Direction){
  int prevReading=0;
  current_pos=0;
  do{
    prevReading = current_pos;
    
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
    current_pos = analogRead(A0);
  }while(prevReading != current_pos);
  return current_pos;
}
