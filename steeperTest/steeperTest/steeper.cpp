#include "command.h"
#include "steeper.h"

motor::motor(int Step_enabled,int Step_pin,int Step_dir,byte *Interrupt_pin,bool sentido,int number){
  
  pinMode(Step_dir, OUTPUT);
  pinMode(Step_pin, OUTPUT);
  pinMode(Step_enabled,OUTPUT);
  digitalWrite(Step_dir,LOW);
  digitalWrite(Step_pin,LOW);
  digitalWrite(Step_enabled,LOW);

 
  this->intT=Interrupt_pin;  
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
   if(*intT==1)
    {
      Stepping = false;
      DISTANCE=StepCounter;
      return -1;
    } 
  if (Stepping == true && DISTANCE!=StepCounter )
  {
    digitalWrite(this->step_pin, HIGH);
    //delay(1); 
    delayMicroseconds(150);         
    digitalWrite(this->step_pin, LOW); 
    //delay(1);
    delayMicroseconds(150);

 
    
    StepCounter = StepCounter + delta;

    if (StepCounter == DISTANCE)
    {
 
      Stepping = false;
      return 0;
    }
  return 1;  
  }
  return 0;
}

int motor::moveto(long position){
    
    if(*intT!=1)
    {
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
    else{
      
      }
    return 0;
        
}

int motor::distancetoGo(){
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
void motor::stop(){

  
}

