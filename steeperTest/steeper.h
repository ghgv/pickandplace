#ifndef STEEPER_H
#define STEEPER_H

#include "Arduino.h"

//static byte state = 0;

class motor{
  public:
    
    int delta=0;
    int number;
    bool sense =true;
    bool Stepping=0;
    long StepCounter;
    volatile byte state;
    volatile int *intT;
    
    float DISTANCE=0;
    int step_pin,step_dir,step_enabled;
    motor(int Step_enabled,int Step_pin,int Step_dir,int Interrupt_pin,bool sentido,int number);
    ~motor(){};  
    int run();
    int cicle();
    int limit();
    int moveto(long position);
    int distancetoGo();
    


  
};




#endif
