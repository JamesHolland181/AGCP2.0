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
int speed_control = 6; // control speed of actuator with PWM from D6
int pwm = 0; // 0-255, control speed of actuator/braking

int potentiometer = A0; // for feedback
int maxAnalogReading; // max val of pot
int minAnalogReading; // min val of pot
bool limit = false; // limit reached?

String command = ""; // --> [ 'B:', 'braking_percentage %', 'braking_speed %' ]
int braking_percentage = 0; // desired braking amount
int braking_speed = 0; // speed of actuator motion


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
    pinMode(potentiometer, INPUT);

    // find limits of potentiometer
    maxAnalogReading = moveToLimit(1);
    minAnalogReading = moveToLimit(-1);
}

unsigned char old_broadcast[8];

void loop(){    
    // handle receiving inputs
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      if (CAN.getCanId() == 9) {         // if CAN is from on-board computer ...
        CAN.readMsgBuf(&len, input); // read can message to update direction
        
      }    
      else
      {         
        input_handler(can_msg_to_input()); // convert can message to input
      }            
    }    
    
     // handle receiving inputs        
    while (Serial.available() > 0){
        char c = Serial.read();
        command += c;
        delay(5);
    }
    
    if(command != ""){
        Serial.print("MSG: ");
        Serial.println(command);
        input_handler(command); // convert can message to input
    }
    
    if(command.substring(0,command.indexOf(":")) == "B")
    {
        Serial.println("Brake");
        command.remove(0,(command.indexOf(":")+1));
        Serial.println(command);
        pwm = map(command.toInt(),0,100,40,255); 

        // Apply brakes
        if(command.toInt() >= 1)
        {
        delay(500);
        Serial.println("on");
        
        // implement feedback
        while(limit == false){
          digitalWrite(speed_control,pwm);
          digitalWrite(engage[0],HIGH);

          // break from loop if limits have been reached
          if((A0 >= maxAnalogReading)or(A0 <= minAnalogReading)){ break;}

          // break from loop if desired braking percentage reached
          if((A0/(maxAnalogReading-minAnalogReading)) == braking_percentage){break;}
        }

        // power down
        digitalWrite(speed_control,0);
        digitalWrite(engage[0],LOW); 
        digitalWrite(engage[1],LOW); 
        
        }
        
        // Release brakes
        else
        {
        delay(500);
        Serial.println("off");
         
        while(limit == false){
          digitalWrite(speed_control,pwm);
          digitalWrite(disengage[0],HIGH);

          if((A0 >= maxAnalogReading)or(A0 <= minAnalogReading)){ break;}
        }

        // power down 
        digitalWrite(speed_control, 0);
        digitalWrite(disengage[0],LOW); 
        digitalWrite(disengage[1],LOW);
        }    

        // implement potentiometer feedback to stop power supply
        delay(2000);
        digitalWrite(disengage[0],LOW);
        digitalWrite(disengage[1],LOW);
    }    
    
    command = "";    
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

// for calibrating linear actuator
int moveToLimit(int Direction){
  int prevReading=0;
  int currReading=0;
  do{
    prevReading = currReading;
    
    if(Direction ==1){
        digitalWrite(speed_control,pwm);
        digitalWrite(engage[0],HIGH);
        digitalWrite(engage[1],LOW);
    }
    else if(Direction == -1){
        digitalWrite(speed_control,pwm);
        digitalWrite(disengage[0],HIGH);
        digitalWrite(disengage[1],LOW);
    }

    delay(200); //keep moving until analog reading remains the same for 200ms
    currReading = analogRead(potentiometer);
  }while(prevReading != currReading);
  return currReading;
}
