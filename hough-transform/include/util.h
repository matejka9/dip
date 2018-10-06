#ifndef _UTIL_H_
#define _UTIL_H_

double normAlpha(double alpha);

double rad_normAlpha(double alpha);

// utility function to compute directional angle from alpha to beta (+/- 180), all values in deg.
short angle_difference(short alpha, short beta);

// utility function to compute directional angle from alpha to beta (+/- 180), all values in deg.
double angle_rad_difference(double alpha, double beta);

#endif
