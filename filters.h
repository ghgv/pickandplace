#ifndef FILTERS_H
#define FILTERS_H


struct blur
{
float a=2,b=2,c=2;
float d=2,e=4,f=2;
float g=2,h=2,i=2;
float div=(a+b+c+d+e+f+g+h+i);
};

struct dith
{
float a=6,b=8,c=4;
float d=1,e=0,f=3;
float g=5,h=2,i=7;
float div=(a+b+c+d+e+f+g+h+i);
};

struct sobelx
{
float a=-1,b=0,c=1;
float d=-2,e=0,f=2;
float g=-1,h=0,i=1;

};

struct sobely
{
float a=-1,b=-2,c=-1;
float d=0,e=0,f=0;
float g=1,h=2,i=1;

};

#endif // FILTERS_H
