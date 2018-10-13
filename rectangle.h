#ifndef RECTANGLE_H
#define RECTANGLE_H

struct point{
    float x;
    float y;
};


class rectangle
{
public:

    point a,b,c,d;
    float angle=0;
    int segxab;
    int segyab;
    int segxbc;
    int segybc;
    int segxcd;
    int segycd;
    int segxda;
    int segyda;
    rectangle(float W,float L,float Angle,int width,int height);
    void segmenta(float t,int *x,int *y);
    void segmentb(float t,int *x,int *y);
    void segmentc(float t,int *x,int *y);
    void segmentd(float t,int *x,int *y);
};

#endif // RECTANGLE_H
