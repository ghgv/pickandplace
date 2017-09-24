#ifndef LEAST_SQUARES_H
#define LEAST_SQUARES_H

#define MAX_POINTS 100

typedef struct {
  double x, y;
} Point2;

int num = 0;
Point2 list[MAX_POINTS];
int circleFitNeedsRecalc = 0;
int showCircle = 1;
int circleInfo = 0;
int windowHeight;
double a, b, r = 0.0;   /* X, Y, and radius of best fit circle.
                         */

#endif // LEAST_SQUARES_H
