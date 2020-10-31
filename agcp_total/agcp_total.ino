String input = ""; // string of command to be acted on
int Motor = 2;     // To send PWM signal to motor controller
int DirectionRelay = 3; // I honestly cannot remember, think it was for referencing which gear is selected?
int ForwardSwitch = 5; // To actuate switch for gear selection on motor controller
int ReverseSwitch = 4; // for reverse gear selection
int LeftTurnSig = 6; // for turning on left blinker
int RightTurnSig = 7; // for right blinker
int BrakeOn = 8;  // for brake circuit, powers on the linear actuator
int BrakeOff = 9; // disengages the brake
int Brake = 0;
void setup() 
{
  // put your setup code here, to run once:
  pinMode(Motor,OUTPUT);
  pinMode(DirectionRelay,OUTPUT);
  pinMode(LeftTurnSig,OUTPUT);
  pinMode(RightTurnSig,OUTPUT);
  pinMode(ForwardSwitch,INPUT);
  pinMode(ReverseSwitch,INPUT);
  pinMode(BrakeOn,OUTPUT);
  pinMode(BrakeOff,OUTPUT);
  digitalWrite(BrakeOn,LOW);
  digitalWrite(BrakeOff,LOW);
  digitalWrite(LeftTurnSig, HIGH);
  digitalWrite(RightTurnSig, HIGH);
  
  Serial.begin(9600);
  
}

void loop() 
{
  int F_Direction = digitalRead(ForwardSwitch);
  int R_Direction = digitalRead(ReverseSwitch);
  int Pedal = analogRead(A0);

  // if a gear is selected (not in neutral)
  while(F_Direction == HIGH || R_Direction == HIGH)
  {
    int F_Direction = digitalRead(ForwardSwitch);
    int R_Direction = digitalRead(ReverseSwitch);
    //Serial.println(F_Direction);
    //Serial.println(R_Direction);
    
    if(F_Direction == HIGH)
    {
      digitalWrite(DirectionRelay, LOW);
    }
    else if(R_Direction == HIGH)
    {
      digitalWrite(DirectionRelay,HIGH);
    }
    else
    {
      analogWrite(Motor,0);
      digitalWrite(DirectionRelay,LOW);
      break;
    }
    Serial.println(Pedal);
    Pedal = analogRead(A0);

    // if someone is pressing the pedal, disengage "autonomous mode"
    if(Pedal < 80)
    {
      analogWrite(Motor,0);
    }
    else
    {
      // convert percentage to range from 0 to 255 (PWM) and send to motor controller
      int pwm = map(Pedal,80,885,0,255);
      if(R_Direction == HIGH) // if in reverse reverse speed
      {
        analogWrite(Motor,pwm-5); 
      }
      else
      {
        analogWrite(Motor,pwm); 
      }
    }
    
  }
  while (Serial.available() > 0) // reading in the input command
  {
    char c = Serial.read();
    input += c;
    delay(5);
  }
  
  if(input != "") // print input to serial
  {
    Serial.print("I received: ");
    Serial.println(input);
  }
  if(input.substring(0,1) == "S") // "S" is the "off" command for the autonomous mode --> kills power to motor
  {
    analogWrite(Motor,0);
    digitalWrite(DirectionRelay,LOW);
  }

  // here the input is parsed to determine what is being asked
  // format --> "Letter:Percentage"
  if(input.substring(0,input.indexOf(":")) == "R")// R for reverse, followed by the percentage of speed --> same function as forward code block
  {
    digitalWrite(DirectionRelay,HIGH); // set direction relay to high, engaging reverse (LOW for forward)
    Serial.println("reverse");
    input.remove(0,(input.indexOf(":")+1));  
    Serial.println(input);
    int pwm = map(input.toInt(),0,100,40,255); // converts the desired percentage to range from 40 to 255
    analogWrite(Motor,pwm); // send PWM signal to motor controller
    Serial.println(pwm);
    
  }
  else if(input.substring(0,input.indexOf(":")) == "F")// F for forward
  {
    digitalWrite(DirectionRelay,LOW);
    Serial.println("forward");
    input.remove(0,(input.indexOf(":")+1));
    Serial.println(input);
    int pwm = map(input.toInt(),0,100,40,255);
    analogWrite(Motor,pwm);
    Serial.println(pwm);
    
  }
  else if(input.substring(0,input.indexOf(":")) == "B") // B for brakes
  {
    Serial.println("Brake");
    input.remove(0,(input.indexOf(":")+1));
    Serial.println(input);
    
    if(input.toInt() == 1) // "1" engages the linear actuator, anything else disengages linear actuator
    {
      delay(500);
      Serial.println("on");
      digitalWrite(BrakeOff,LOW);
      delay(500);
      digitalWrite(BrakeOn,HIGH);
    }
    else
    {
      delay(500);
      Serial.println("off");
      digitalWrite(BrakeOn,LOW);
      delay(500);
      digitalWrite(BrakeOff,HIGH);
    }    

    delay(2000);
    digitalWrite(BrakeOn,LOW);
    digitalWrite(BrakeOff,LOW);
  }

  // the following is for turn signals: LL for left, RL for right, FL for hazards, SL for off
  else if(input.substring(0,2) == "LL")
  {
    Serial.println("Left");
    digitalWrite(LeftTurnSig,LOW);
    digitalWrite(RightTurnSig,HIGH);
  }
  else if(input.substring(0,2) == "RL")
  {
    Serial.println("Right");
    digitalWrite(RightTurnSig,LOW);
    digitalWrite(LeftTurnSig,HIGH);
  }
  else if(input.substring(0,2) == "FL")
  {
    Serial.println("Flash");
    digitalWrite(RightTurnSig,LOW);
    digitalWrite(LeftTurnSig,LOW);
  }
  else if(input.substring(0,2) == "SL")
  {
    Serial.println("Flash off");
    digitalWrite(RightTurnSig,HIGH);
    digitalWrite(LeftTurnSig,HIGH);
  }
  
  
  input = ""; // clearing input buffer
}
