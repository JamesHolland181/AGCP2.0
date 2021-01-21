#include <SPI.h>
#include "mcp2515_can.h"

const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

unsigned char broadcast[1] = {0};
unsigned char acceleration[2] = {0,2}; // tps, current_dir
unsigned char brake[3] = {0,0,0};        // braking_dir, braking_percentage, braking_speed
unsigned char steering[2] = {0,0};     // rot_dir, rot_speed
unsigned char transmission[1] = {2}; // desired_gear

String request = "";
String node = "";
bool flag = false;

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){      
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

void loop() {
    // handle receiving user inputs
    //Serial.println("Enter what system to send command to");
    while (Serial.available() > 0) {
      //Serial.println("reading");
      char c = Serial.read();
      if (c != '\n') {
        node += c;
        flag = true;
      }
      delay(5);
    }
    
    if(node != ""){broadcast[1] = node.toInt();}
    
    CAN.sendMsgBuf(9, 0, sizeof(broadcast), broadcast);
    SERIAL_PORT_MONITOR.println("Acceleration message send!");

//    switch(node.toInt()){
//      case 1:
//        CAN.sendMsgBuf(9, 0, 2, acceleration);
//        SERIAL_PORT_MONITOR.println("Acceleration message send!");
//        break;
//
//      case 2:
//        CAN.sendMsgBuf(9, 0, 1, transmission);
//        SERIAL_PORT_MONITOR.println("Gear selection message send!");
//        break; 
//        
//      case 3:
//        CAN.sendMsgBuf(9, 0, 3, brake);
//        SERIAL_PORT_MONITOR.println("Brake message send!");
//        break;
//
//      case 4:
//        CAN.sendMsgBuf(9, 0, 2, steering);
//        SERIAL_PORT_MONITOR.println("Steering message send!");
//        break;               
//    }

    delay(100);                       // send data per 100ms
}
