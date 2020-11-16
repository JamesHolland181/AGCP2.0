/*********************************************************************************************************************/
/* Author: James C. Holland
/* Date: 11/15/2020
/* 
/* Purpose: Controlling cart via CAN system.
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

///////
//
// CAN-Bus Preamble
// 
///////

const int ID = 9; // on-board node ID
String user_input = ""; // CAN message content

bool gathering_inputs = 0;
unsigned char send_id[1] = {'0'};
String str_send_id = "";
String send_msg = "";

String can_id = "";  // CAN sender's ID

void setup() {
    // CAN-BUS Setup
    SERIAL.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println("Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("<=========================================================>");
}

void loop() {
    user_input = " "; // reset user_input
    // handle monitoring CAN messages
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
        CAN.readMsgBuf(&len, input); // read can message to update direction

        Serial.print("CAN >> ");
        Serial.print("\tID: ");
        Serial.print(CAN.getCanId());
        Serial.print("\tMSG: "); 
        Serial.println(char(input));
        Serial.println("<=========================================================>");
    }    

    while(Serial.available() > 0){
        user_input = Serial.readString();
        delay(5);
    }

    if(user_input != " "){
        Serial.println("\nuser_input: "+user_input);
        user_input.replace("\n","");
    }

    if(user_input == "help"){
        Serial.println("Figure it out yourself . . .");
    }
    else if(user_input == "command"){
        gathering_inputs = 1;
        Serial.println("Gathering inputs . . .");
    }
    else if((user_input.toInt() > 0) and (user_input.toInt() < 10)){
        //char buf[20];
        str_send_id = user_input;
        user_input.toCharArray(send_id,8);
        Serial.print("ID: ");
        Serial.println(user_input.toInt());
    }
    else if(user_input.substring(0,7)=="message"){
        Serial.println("Message...");

        switch(str_send_id.toInt()){
          // accelerator
          case 1:
          break;

          // gear selector
          case 2:
          break;
          
          // brake
          case 3:
          break;


          // steering
          case 4:
          break;


          // lights
          case 5:
          break;        
            
          default:
          break;
        }
    }

    if((gathering_inputs == 1)and(user_input != "")and(send_id[0] != '0')and(send_msg != "")){
        Serial.println("Inputs gathered . . .");
        send_msg.toCharArray(broadcast,8);
        Serial.print("Broadcast: ");
        for(int i=0; i<9 ; i++){
          Serial.print(broadcast[i]);
        }
        Serial.println("\nSending broadcast . . .");
        CAN.sendMsgBuf(send_id, 0, 8, broadcast);     
        gathering_inputs=0;
        user_input="";
        send_id[0]='0';
        send_msg="";
    }
    
}
