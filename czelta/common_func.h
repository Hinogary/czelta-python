/*
 * Author: Martin Quarda
 */
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#ifndef COMMONFUNC_H
#define	COMMONFUNC_H
#define SPEED_OF_LIGHT 299792458.0
#define TIMESTAMP_ZERO_SIDEREAL 83124

#ifdef __cplusplus
extern "C"{
#endif

//haversine method
double deltaDistance(double* f_gps_pos, double* s_gps_pos);
float* dirVectorToAh(double* vector);
time_t char_date(const char*);
time_t date(int year, int month, int day, int hour, int minute, int second);
double getJulianFromUnix(time_t unixSecs);
double lSideRealFromUnix(time_t unixSecs, float degres_longtitude);
float* localToGlobalDirection(float* local_direction, double* gps_position, time_t time);
float* localToAGlobalDirection(float* local_direction, double* gps_position);

#ifdef __cplusplus
}

#include <string>
inline time_t string_date(std::string date){return char_date(date.c_str());};
inline time_t date(int year, int month, int day){return date(year, month, day, 0, 0, 0);};
inline time_t date(int year, int month, int day, int hour, int minute){return date(year, month, day, hour, minute, 0);};
#endif

#endif	/* COMMONFUNC_H */
