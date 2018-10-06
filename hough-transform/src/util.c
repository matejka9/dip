#include <math.h>

#include "util.h"

double normAlpha(double alpha){
   if(alpha < 0){
      while(alpha < 0)
         alpha += 360;
   }
   else
      while(alpha >= 360)
         alpha -= 360;
   return alpha;
}

double rad_normAlpha(double alpha){
   if(alpha < 0){
      while(alpha < 0)
         alpha += 2.0 * M_PI;
   }
   else
      while(alpha >= 2.0 * M_PI)
         alpha -= 2.0 * M_PI;
   return alpha;
}

short angle_difference(short alpha, short beta)
{
  short diff = beta - alpha;
  if (diff > 180) return diff - 360;
  else if (diff < -180) return diff + 360;
  return diff;
}

double angle_rad_difference(double alpha, double beta)
{
  beta = rad_normAlpha(beta);
  alpha = rad_normAlpha(alpha);
  double diff = beta - alpha;
  if (diff > M_PI) return diff - 2.0 * M_PI;
  else if (diff < -M_PI) return diff + 2.0 * M_PI;
  return diff;
}
