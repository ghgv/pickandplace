#include "least_squares.h"
#include <math.h>

/****************************************************************************
   Least squares fit of circle to set of points.
   by Dave Eberly (eberly@cs.unc.edu or eberly@ndl.com)
   ftp://ftp.cs.unc.edu/pub/users/eberly/magic/circfit.c
  ---------------------------------------------------------------------------
   Input:  (x_i,y_i), 1 <= i <= N, where N >= 3 and not all points
           are collinear
   Output:  circle center (a,b) and radius r

   Energy function to be minimized is

      E(a,b,r) = sum_{i=1}^N (L_i-r)^2

   where L_i = |(x_i-a,y_i-b)|, the length of the specified vector.
   Taking partial derivatives and setting equal to zero yield the
   three nonlinear equations

   E_r = 0:  r = Average(L_i)
   E_a = 0:  a = Average(x_i) + r * Average(dL_i/da)
   E_b = 0:  b = Average(y_i) + r * Average(dL_i/db)

   Replacing r in the last two equations yields

     a = Average(x_i) + Average(L_i) * Average(dL_i/da) = F(a,b)
     b = Average(y_i) + Average(L_i) * Average(dL_i/db) = G(a,b)

   which can possibly be solved by fixed point iteration as

     a_{n+1} = F(a_n,b_n),  b_{n+a} = G(a_n,b_n)

   with initial guess a_0 = Average(x_i) and b_0 = Average(y_i).
   Derivative calculations show that

     dL_i/da = (a-x_i)/L_i,  dL_i/db = (b-y_i)/L_i.

  ---------------------------------------------------------------------------
   WARNING.  I have not analyzed the convergence properties of the fixed
   point iteration scheme.  In a few experiments it seems to converge
   just fine, but I do not guarantee convergence in all cases.
 ****************************************************************************/

int
CircleFit(int N, Point2 * P, double *pa, double *pb, double *pr)
{
  /* user-selected parameters */
  const int maxIterations = 256;
  const double tolerance = 1e-06;

  double a, b, r;

  /* compute the average of the data points */
  int i, j;
  double xAvr = 0.0;
  double yAvr = 0.0;

  for (i = 0; i < N; i++) {
    xAvr += P[i].x;
    yAvr += P[i].y;
  }
  xAvr /= N;
  yAvr /= N;

  /* initial guess */
  a = xAvr;
  b = yAvr;

  for (j = 0; j < maxIterations; j++) {
    /* update the iterates */
    double a0 = a;
    double b0 = b;

    /* compute average L, dL/da, dL/db */
    double LAvr = 0.0;
    double LaAvr = 0.0;
    double LbAvr = 0.0;

    for (i = 0; i < N; i++) {
      double dx = P[i].x - a;
      double dy = P[i].y - b;
      double L = sqrt(dx * dx + dy * dy);
      if (fabs(L) > tolerance) {
        LAvr += L;
        LaAvr -= dx / L;
        LbAvr -= dy / L;
      }
    }
    LAvr /= N;
    LaAvr /= N;
    LbAvr /= N;

    a = xAvr + LAvr * LaAvr;
    b = yAvr + LAvr * LbAvr;
    r = LAvr;

    if (fabs(a - a0) <= tolerance && fabs(b - b0) <= tolerance)
      break;
  }

  *pa = a;
  *pb = b;
  *pr = r;

  return (j < maxIterations ? j : -1);
}
