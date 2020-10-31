String input = "";

//acceleration_control variable
int Motor = 2;

//gearSelector_control variables
int DirectionRelay = 3;
int ForwardSwitch = 5;
int ReverseSwitch = 4;

//turnSignal_control variables
int LeftTurnSig = 6;
int RightTurnSig = 7;

//not determined yet
int BrakeOn = 8;
int BrakeOff = 9;
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
  
  // not assigned yet
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
  
  
// controlling acceleration occurs here:
// two systems involved --> gearSelector and acceleration
// gearSelector reads what gear is selected and outputs to the motor which way to apply power (fwd or rev)

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
    
    if(Pedal < 80)
    {
      analogWrite(Motor,0);
    }
    else
    {
      int pwm = map(Pedal,80,885,0,255);
      if(R_Direction == HIGH)
      {
        analogWrite(Motor,pwm-5); 
      }
      else
      {
        analogWrite(Motor,pwm); 
      }
    }
    
  }
  
// handling received inputs  
  while (Serial.available() > 0) 
  {
    char c = Serial.read();
    input += c;
    delay(5);
  }
  
  if(input != "")
  {
    Serial.print("I received: ");
    Serial.println(input);
  }
  
// aceleration  
  if(input.substring(0,1) == "S")
  {
    analogWrite(Motor,0);
    digitalWrite(DirectionRelay,LOW);
  }
  
  if(input.substring(0,input.indexOf(":")) == "R")
  {
    digitalWrite(DirectionRelay,HIGH);
    Serial.println("reverse");
    input.remove(0,(input.indexOf(":")+1));  
    Serial.println(input);
    int pwm = map(input.toInt(),0,100,40,255);
    analogWrite(Motor,pwm);
    Serial.println(pwm);
    
  }
  else if(input.substring(0,input.indexOf(":")) == "F")
  {
    digitalWrite(DirectionRelay,LOW);
    Serial.println("forward");
    input.remove(0,(input.indexOf(":")+1));
    Serial.println(input);
    int pwm = map(input.toInt(),0,100,40,255);
    analogWrite(Motor,pwm);
    Serial.println(pwm);
    
  }
  
// braking  
  else if(input.substring(0,input.indexOf(":")) == "B")
  {
    Serial.println("Brake");
    input.remove(0,(input.indexOf(":")+1));
    Serial.println(input);
    
    if(input.toInt() == 1)
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
//

// turn signals
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
//  
  
  input = "";
}
