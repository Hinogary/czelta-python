/*
 * File:   commonFunc.cpp
 * Author: martin
 *
 * Created on 5. listopad 2013, 21:15
 */

#include <string.h>
#include <iostream>
#include <time.h>

#include "common_func.h"

double deltaDirection(double hor1, double az1, double hor2, double az2) {
    double rtn = acos(sin(hor1) * sin(hor2) + cos(hor1) * cos(hor2) * cos(az1 - az2));
    if(rtn!=rtn)return 0;
    return rtn;
}

time_t date(string date) {
    struct tm tm;
    if (strptime(date.c_str(), "%d.%m.%Y %H:%M:%S", &tm) != NULL){
        tm.tm_hour++;
        return mktime(&tm);
    }
    if (strptime(date.c_str(), "%d.%m.%Y %H:%M", &tm) != NULL){
        tm.tm_hour++;
        return mktime(&tm);
    }
    if (strptime(date.c_str(), "%d.%m.%Y %H", &tm) != NULL){
        tm.tm_hour++;
        return mktime(&tm);
    }
    if (strptime(date.c_str(), "%d.%m.%Y", &tm) != NULL){
        tm.tm_hour++;
        return mktime(&tm);
    }
    return 0;
}

time_t date(int year, int month, int day) {
    return date(year, month, day, 0, 0, 0);
}

time_t date(int year, int month, int day, int hour, int minute) {
    return date(year, month, day, hour, minute, 0);

}

time_t date(int year, int month, int day, int hour, int minute, int second) {
    struct tm tm;
    memset(&tm,0,sizeof(tm));
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour+1;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;
    return mktime(&tm);
}
