/*
 * Author: Martin Quarda
 */
#include <math.h>
#include <string>
#include <stdio.h>
#include <array>
#ifndef COMMONFUNC_H
#define	COMMONFUNC_H
using namespace std;

double deltaDirection(double hor1, double az1, double hor2, double az2);
inline double deltaDirection(array<float,2> a, array<float,2> b){return deltaDirection(a[0],a[1],b[0],b[1]);};
time_t date(string date);
time_t date(int year, int month, int day);
time_t date(int year, int month, int day, int hour, int minute);
time_t date(int year, int month, int day, int hour, int minute, int second);
double getJulianFromUnix(time_t unixSecs);
double lSideRealFromUnix(time_t unixSecs, float degres_longtitude);
float* localToGlobalDirection(float* local_direction, float* gps_position);
#endif	/* COMMONFUNC_H */

