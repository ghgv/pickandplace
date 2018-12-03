
#include "command.h"
#include <U8g2lib.h>
#include "steeper.h"
//#include <AccelStepper.h>
#include "output.h"
#include "termistor.h"

#define TEMP_0_PIN         13   // Analog Input



#define E0_STEP_PIN         26 //
#define E0_DIR_PIN          28
#define E0_ENABLE_PIN       24
#define E0_MIN_PIN          999 

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Z_STEP_PIN         60
#define Z_DIR_PIN          61
#define Z_ENABLE_PIN       56
#define Z_MIN_PIN          14
#define Z_MAX_PIN          15

#define Y_STEP_PIN         46
#define Y_DIR_PIN          48
#define Y_ENABLE_PIN       62
#define Y_MIN_PIN          18
#define Y_MAX_PIN          19
const byte BUTTON =3;

#define PUMP_LARGE               9  //The pump
#define PUMP_SMALL               8  //The pump
int a;
volatile int intE;
volatile byte intX=0,intY,intZ;
float xvalue,yvalue;
float Factor2=1;
float microstepsG=400;  //factor to adjust movements of the axis


int StepCounter = 0;
int Stepping = false;
motor E0(E0_ENABLE_PIN,E0_STEP_PIN,E0_DIR_PIN,E0_MIN_PIN,1,4);
motor X(X_ENABLE_PIN,X_STEP_PIN,X_DIR_PIN,&intX,0,1);
motor Y(Y_ENABLE_PIN,Y_STEP_PIN,Y_DIR_PIN,&intY,1,2);
motor Z(Z_ENABLE_PIN,Z_STEP_PIN,Z_DIR_PIN,&intZ,1,3);

#define LCD_PINS_RS     16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4     23
#define LIGHTS          10

#define ST7920_CLK_PIN  LCD_PINS_D4
#define ST7920_DAT_PIN  LCD_PINS_ENABLE
#define ST7920_CS_PIN   LCD_PINS_RS


U8G2_ST7920_128X64_F_SW_SPI u8g(U8G2_R0,ST7920_CLK_PIN, ST7920_DAT_PIN, ST7920_CS_PIN,8);  
void draw(void){
 
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 22, "www.dali.com.co"); 
}



void setup() {                
  

  Serial.println("PnP v1.0 8OCT2018");
  cmdInit(115200);
  
  cmdAdd("pos",positioner);
  cmdAdd("M17", M17);
  cmdAdd("M18", M18);
  cmdAdd("M3", M3);
  cmdAdd("M4", M4);
  cmdAdd("M32", M32);
  cmdAdd("M42", M42);
  cmdAdd("M6",M6);
  cmdAdd("G0",G0);
  cmdAdd("G1",G1);
  cmdAdd("G28",G28);
  cmdAdd("G92",G92);
  cmdAdd("M112",M112);
  cmdAdd("M114",M114);
  cmdAdd("Lights",LIGHT);
  cmdAdd("M999",M999);//Restart
  
  pinMode(PUMP_LARGE, OUTPUT);           
  pinMode(PUMP_SMALL, OUTPUT);           
  pinMode(LIGHTS, OUTPUT);           
  digitalWrite(PUMP_LARGE, HIGH);  
  digitalWrite(PUMP_SMALL, HIGH);       
  

  // flip screen, if required
  // u8g.setRot180();
   u8g.begin();
  // u8g.drawStr(0,11,"www.dali.com.co");

   pinMode(X_MIN_PIN, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(X_MIN_PIN), Xlimit, HIGH);
   pinMode(Y_MIN_PIN, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(Y_MIN_PIN), Ylimit, HIGH);
   pinMode(Z_MIN_PIN, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(Z_MIN_PIN), Zlimit, HIGH);
   u8g.clearBuffer();
   u8g.setFont(u8g_font_6x12);
   u8g.drawStr(0,11,"www.dali.com.co");
   u8g.drawStr(0,22,"Temperatura ambiente: ");
   //u8g.drawStr(0,32,String(celsius,10).c_str());
   u8g.sendBuffer();
   
   
   *X.intT=0;
   *Y.intT=0;
   *Z.intT=0;
     pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  cmdPoll();
  E0.run();
  X.run();
  Y.run();
  Z.run();
 

   /*int rawvalue = analogRead(TEMP_0_PIN);
   int celsius = read_temp();
   int fahrenheit = (((celsius * 9) / 5) + 32);*/
    //delay(1000);*/
  
}

void M17(int arg_cnt, char **args)//Enabled
{
       digitalWrite(Z_ENABLE_PIN, 0);
       digitalWrite(Y_ENABLE_PIN, 0);
       digitalWrite(X_ENABLE_PIN, 0);
       Serial.println("Motors enabled");

}

void M18(int arg_cnt, char **args)//Disabled
{
       digitalWrite(Z_ENABLE_PIN, 1);
       digitalWrite(Y_ENABLE_PIN, 1);
       digitalWrite(X_ENABLE_PIN, 1);
       Serial.println("Motors disabled");
       u8g.drawStr(0,22,"Motors disabled");
       u8g.sendBuffer();
  
}




void positioner(int arg_cnt,char **args)//pos
{
  xvalue = atof(args[1]);
  yvalue = atof(args[2]);

 if(strcmp(args[1],"E")==0)
    {
      while(E0.distancetoGo()!=0){
        E0.run();
        }
      E0.moveto(yvalue*microstepsG/Factor2);
      
      while(E0.distancetoGo()!=0)
        E0.run();
      Serial.println("ok");
     
    }

if(strcmp(args[1],"X")==0)
    {
      while(X.distancetoGo()!=0){
        if(intX==1)
          X.stop();
        X.run();
        }
      X.moveto(yvalue*microstepsG/Factor2);
      
      while(X.distancetoGo()!=0)
        X.run();
      Serial.println("ok");
     
    }

if(strcmp(args[1],"Y")==0)
    {
      while(Y.distancetoGo()!=0){
        Y.run();
        }
      Y.moveto(yvalue*microstepsG/Factor2);
      
      while(Y.distancetoGo()!=0)
        Y.run();
      Serial.println("ok");
     
    }
if(strcmp(args[1],"Z")==0)
    {
      while(Z.distancetoGo()!=0){
        Z.run();
        }
      Z.moveto((long)(yvalue*microstepsG/Factor2));
     
      
      while(Z.distancetoGo()!=0)
        Z.run();
      Serial.println("ok");
     
    }    


  }



int read_temp()
{
   int rawtemp = analogRead(TEMP_0_PIN);
   int current_celsius = 0;

   byte i;
   for (i=1; i<NUMTEMPS; i++)
   {
      if (temptable[i][0] > rawtemp)
      {
         int realtemp  = temptable[i-1][1] + (rawtemp - temptable[i-1][0]) * (temptable[i][1] - temptable[i-1][1]) / (temptable[i][0] - temptable[i-1][0]);

         if (realtemp > 255)
            realtemp = 255; 

         current_celsius = realtemp;

         break;
      }
   }

   // Overflow: We just clamp to 0 degrees celsius
   if (i == NUMTEMPS)
   current_celsius = 0;

   return current_celsius;
}


void G1(int arg_cnt,char **args)//G0
{
  int i=1;
  int n=0;
  Serial.print("G1 ");
  String command[5];
  while(args[i]!=NULL){
    command[i]=args[i];
    Serial.print(args[i]);
    Serial.print(" ");
    i++;
  }
  
  n=i;
  i=1;
  
  while(i<n){
     if(command[i].charAt(0)=='Z'){
      yvalue = atof(command[i].substring(1).c_str());
      while(Z.distancetoGo()!=0){
        Z.run();
        }
      Z.moveto((long)(yvalue*3200/(3.2416*12.73)));
      
      while(Z.distancetoGo()!=0)
        Z.run();
      Serial.println("ok");
    }

    
    if(command[i].charAt(0)=='X'){
      yvalue = atof(command[i].substring(1).c_str());
      while(X.distancetoGo()!=0){
        X.run();
        }
      X.moveto(yvalue*microstepsG/Factor2);
      
      while(X.distancetoGo()!=0)
        X.run();
      Serial.println("ok");
    }
    
    if(command[i].charAt(0)=='Y'){
      yvalue = atof(command[i].substring(1).c_str());
      while(Y.distancetoGo()!=0){
        Y.run();
        }
      Y.moveto(yvalue*microstepsG/Factor2);
      
      while(Y.distancetoGo()!=0)
        Y.run();
      Serial.println("ok");
    }
    


    if(command[i].charAt(0)=='E' ){
      yvalue = atof(command[i].substring(1).c_str());
      while(E0.distancetoGo()!=0){
        E0.run();
        }
      E0.moveto(yvalue*3200/360);
      
      while(E0.distancetoGo()!=0)
        E0.run();
      Serial.println("ok");
    }
 
    i++;
  }
  
    
}


void G0(int arg_cnt,char **args)//G1
{
  int i=1;
  while(args[i++]!=NULL){
    Serial.println(args[i]);
  }
}

void M3(int arg_cnt,char **args){//Laser On
   digitalWrite(PUMP_LARGE, LOW);
   Serial.println("ok");
}


void M4(int arg_cnt,char **args){//Laser On
   digitalWrite(PUMP_LARGE, HIGH); 
   Serial.println("ok");
}


void M32(int arg_cnt,char **args){//Pump Large  off
   digitalWrite(PUMP_SMALL, HIGH); 
   Serial.println("ok");
}

void M42(int arg_cnt,char **args){//Pump Small On
   digitalWrite(PUMP_SMALL, LOW);
   Serial.println("ok");
}



void M6(int arg_cnt,char **args){//microstepsG
   microstepsG = atoi(args[1]);
   Serial.println("ok");
}


void M114(int arg_cnt,char **args){//Current Position
  Serial.print("X= ");
  Serial.println(X.StepCounter);
  Serial.print("  Limit= ");
  Serial.println(X.state);
  Serial.print("Y= ");
  Serial.println(Y.StepCounter);
  Serial.print("  Limit= ");
  Serial.println(Y.state);
  Serial.print("Z= ");
  Serial.println(Z.StepCounter);
  Serial.print("  Limit= ");
  Serial.println(Z.state);
  Serial.print("Factor= ");
  Serial.println(microstepsG);
  String out= "ok M114:X" +String(*(X.intT))+", Y"+String(*(Y.intT))+", Z"+String(*(Z.intT)); 
  Serial.print(out.c_str());
}


void G92(int arg_cnt,char **args)//Set Coordinate
{
  int i=1;
  int n=0;
    Serial.print("G92 args: ");
  String command[5];
  while(args[i]!=NULL){
    command[i]=args[i];
    Serial.println(args[i]);
    
    i++;
  }
  
  n=i;
  i=1;
  
  while(i<n){
    
    if(command[i].charAt(0)=='X'){
      yvalue = atof(command[i].substring(1).c_str());
      X.DISTANCE=0;
      X.StepCounter=0;
 
    }
    
    if(command[i].charAt(0)=='Y'){
      yvalue = atof(command[i].substring(1).c_str());
      Y.DISTANCE=0;
      Y.StepCounter=0;

    }
    
    if(command[i].charAt(0)=='Z'){
      yvalue = atof(command[i].substring(1).c_str());
      Z.DISTANCE=0;
      Z.StepCounter=0;

    }
    i++;
  }
   
 String out= "ok G92:X" +String(X.StepCounter)+", Y"+String(Y.StepCounter)+", Z"+String(Z.StepCounter); 
 Serial.print(out.c_str());
 
    
}

void G28(int arg_cnt,char **args)//G1
{
  int i=1;  
  while(args[i++]!=NULL){
    Serial.println(args[i]);
  }
  Serial.print(intX);
  Serial.print(intY);
  Serial.print(intZ);
  Serial.print(*(X.intT));
}

void M112(int arg_cnt,char **args)//Missing implementing a lot of shutdowns
{
  intX=intY=intZ=1;
  Serial.println("Emergency stop");
}

void M999(int arg_cnt,char **args)//Restart 
{
  intX=intY=intZ=0;
  Serial.println("Restart");
}

void LIGHT(int arg_cnt,char **args)//Ligths
{
   if(strcmp(args[1],"ON")==0){
        digitalWrite(LIGHTS, HIGH);
        Serial.println("ok");
   }
    
   if(strcmp(args[1],"OFF")==0){
        digitalWrite(LIGHTS, LOW);
        Serial.println("ok");
   }
}

void Xlimit(){
  intX=1;
  Serial.println("Limit X reached");
}

void Ylimit(){
  //intY=1;
  Serial.println("Limit Y reached");

}

void Zlimit(){
  intZ=1;
  Serial.println("Limit Z reached");

}

