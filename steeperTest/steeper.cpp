#include <Cmd.h>
#include "steeper.h"

motor::motor(int Step_enabled,int Step_pin,int Step_dir,int Interrupt_pin,bool sentido,int number){
  
  pinMode(Step_dir, OUTPUT);
  pinMode(Step_pin, OUTPUT);
  pinMode(Step_enabled,OUTPUT);
  digitalWrite(Step_dir,LOW);
  digitalWrite(Step_pin,LOW);
  digitalWrite(Step_enabled,LOW);

 
  this->intT=0;  
  this->step_pin=Step_pin;
  this->step_dir=Step_dir;
  this->step_enabled=Step_enabled;
  this->state=0;
  this->number=number;
  if(sentido==true)
    sense=HIGH;
  else
    sense=LOW;
  
}


int motor::run(){
  
    
  if (Stepping == true && DISTANCE!=StepCounter )
  {
    digitalWrite(this->step_pin, HIGH);
    //delay(1); 
    delayMicroseconds(50);         
    digitalWrite(this->step_pin, LOW); 
    //delay(1);
    delayMicroseconds(50);

    //  Serial.print("RUN: ");
    //Serial.println(this->DISTANCE-StepCounter);
    
    StepCounter = StepCounter + delta;

    if (StepCounter == DISTANCE)
    {
      //StepCounter = 0;
      Stepping = false;
      return 0;
    }
  return 1;  
  }
  return 0;
}

int motor::moveto(long position){
    
    
     this->DISTANCE=position;
     if(DISTANCE>=StepCounter){
      digitalWrite(this->step_dir, sense);
      delta=1;
    }
    if(DISTANCE<StepCounter){
      digitalWrite(this->step_dir, !sense);
      delta=-1;
    }
    Stepping=true;
    return 1; 
        
}

int motor::distancetoGo(){
     // Serial.print("DTG: ");
     // Serial.println(this->DISTANCE-StepCounter);
      return this->DISTANCE-StepCounter;
   
  
}

int motor::cicle(){
    digitalWrite(this->step_pin, HIGH);
    delayMicroseconds(50);         
    digitalWrite(this->step_pin, LOW); 
    delayMicroseconds(50);  
}

int motor::limit(){
    state=1;
    DISTANCE=StepCounter=0;
    Serial.print("Interrupted: ");
    //Serial.println(state);

}

