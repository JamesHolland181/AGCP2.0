#include <SPI.h>
#include "mcp2515_can.h"

const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){      
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

unsigned char acceleration[2] = {100,3}; // tps, current_dir
unsigned char brake[3] = {};        // braking_dir, braking_percentage, braking_speed
unsigned char steering[2] = {};     // rot_dir, rot_speed
unsigned char transmission[1] = {}; // desired_gear

void loop() {
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf


    CAN.sendMsgBuf(9, 0, 2, acceleration);
    SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");
    delay(100);                       // send data per 100ms
}

// END FILE
