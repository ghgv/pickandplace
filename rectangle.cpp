#include "rectangle.h"
#include <QtDebug>
#include <iostream>
using namespace std;

rectangle::rectangle(float W,float L,float Angle,int width,int height)
{

    float ax,ay,bx,by,cx,cy,dx,dy;
    this->angle=Angle*3.1415/180;
    W=W*width/38;
    L=L*width/38;
    ax=-W/2;
    ay= L/2;
    bx= W/2;
    by= L/2;
    cx= W/2;
    cy=-L/2;
    dx=-W/2;
    dy=-L/2;

    this->a.x=cos(angle)*(ax)-sin(angle)*(ay);
    this->a.y=sin(angle)*(ax)+cos(angle)*(ay);

    this->b.x=cos(angle)*(bx)-sin(angle)*(by);
    this->b.y=sin(angle)*(bx)+cos(angle)*(by);

    this->c.x=cos(angle)*(cx)-sin(angle)*(cy);
    this->c.y=sin(angle)*(cx)+cos(angle)*(cy);

    this->d.x=cos(angle)*(dx)-sin(angle)*(dy);
    this->d.y=sin(angle)*(dx)+cos(angle)*(dy);

    this->a.x=this->a.x+width/2;
    this->a.y=this->a.y+height/2;
    this->b.x=this->b.x+width/2;
    this->b.y=this->b.y+height/2;
    this->c.x=this->c.x+width/2;
    this->c.y=this->c.y+height/2;
    this->d.x=this->d.x+width/2;
    this->d.y=this->d.y+height/2;



 segxab=this->b.x-this->a.x;
 segyab=this->b.y-this->a.y;
 segxbc=this->c.x-this->b.x;
 segybc=this->c.y-this->b.y;
 segxcd=this->d.x-this->c.x;
 segycd=this->d.y-this->c.y;
 segxda=this->a.x-this->d.x;
 segyda=this->a.y-this->d.y;

}

void rectangle::segmenta(float t,int *x,int *y){
    *x=(int)(a.x+t*segxab);
    *y=(int)(a.y+t*segyab);

}

void rectangle::segmentb(float t,int *x,int *y){
    *x=(int)(b.x+t*segxbc);
    *y=(int)(b.y+t*segybc);

}

void rectangle::segmentc(float t,int *x,int *y){
    *x=(int)(c.x+t*segxcd);
    *y=(int)(c.y+t*segycd);

}

void rectangle::segmentd(float t,int *x,int *y){
    *x=(int)(d.x+t*segxda);
    *y=(int)(d.y+t*segyda);

}

