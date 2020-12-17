#include <digitalWriteFast.h>

#define left_encoder A0
#define right_encoder A1
#define pulse 6
#define dir 5

String request = "";

void setup()
{  
  // Experimental --> uses "digitalWriteFast" library for faster timing
//  pinModeFast(left_encoder,INPUT);
//  pinModeFast(right_encoder,INPUT);
//  pinModeFast(pulse,OUTPUT);
//  pinModeFast(dir,OUTPUT);  
  Serial.begin(115200);
  pinMode(pulse,OUTPUT);
  pinMode(dir,OUTPUT);
  pinMode(left_encoder,INPUT);
  pinMode(right_encoder,INPUT);
}

void loop() 
{           
    // handle receiving user inputs --> simply direction or speed at the moment
    while (Serial.available() > 0){
        char c = Serial.read();
        if(c != '\n'){
          request += c;
        }
        else{Serial.println(request);}
        delay(5);
    }
    // Parse input --> Forward and Reverse
    if(request=="forward"){
      Serial.println(request);
        digitalWrite(dir,HIGH);
    }
    else if(request == "reverse"){
      Serial.println(request);
        digitalWrite(dir,LOW);
    }
    //uncomment if running manually
    for(int x=0;x<20;x++){
//        digitalWriteFast(pulse,HIGH);
//        delay(1); // Test without this as well
//        digitalWriteFast(pulse,LOW);
        
        digitalWrite(pulse,HIGH);
        delay(1); // Test without this as well
        digitalWrite(pulse,LOW);        
    }
    request = "";
}
