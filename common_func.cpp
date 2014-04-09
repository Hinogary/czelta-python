/*
 * Author: Martin Quarda
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
    tm.tm_isdst = 0;
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, sec = 0;
    int readed = sscanf(date.c_str(), "%i.%i.%i %i:%i:%i",&day, &month, &year, &hour, &minute, &sec);
    if(readed<3)return 0;
    tm.tm_year = year-1900;
    tm.tm_mon = month-1;
    tm.tm_mday = day;
    tm.tm_hour = hour+1;
    tm.tm_min = minute;
    tm.tm_sec = sec;
    return mktime(&tm);
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

double getJulianFromUnix( time_t unixSecs ){
   return ( unixSecs / 86400.0 ) + 2440587.5;
}
double lSideRealFromUnix(time_t unixSecs, float degres_longtitude){
//returned time is in radians

//sidereal time 0:00:00 1.1.1970 on longtitude 0
#define ZERO_SIDEREAL_TIME (24054.40168883)
#define SIDEREAL_DAY_TO_DAY (1.002737909350795)
    double side_real_unix = unixSecs*SIDEREAL_DAY_TO_DAY+ZERO_SIDEREAL_TIME;
    time_t side_real_days = floor(side_real_unix/86400.0);
    return side_real_unix-side_real_days*86400;
}

float* localToGlobalDirection(float* local_direction, float* gps_position){
    static float rtn[2];
    rtn[0] = 0;
    rtn[1] = 1;
// sin(declination) = sin(horizon) * sin(longitude) - cos(horizon) * cos(azimut) * cos(longtitude)
// cos(some time) = (sin(horizon) * cos(longtitude) + cos(azimut) * sin(longtitude))/cos(declination)
// sin(some time) = cos(horizon) * sin(azimut) / cos(declination)
// right ascention = SideReal local time - some time
    rtn[0] = acos(sin(local_direction[0])*sin(gps_position[1]) -
                cos(local_direction[0])*cos(local_direction[1])*cos(gps_position[1]));
    return rtn;
}

#ifdef _WIN32
//own light mktime, and gmtime because standart library of windows is ...
#define YEAR0           1900                    /* the first year */
#define EPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SECS_DAY        (24L * 60L * 60L)
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define TIME_MAX        ULONG_MAX
#define ABB_LEN         3
const int _ytab[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
struct tm *
gmtime(register const time_t *timer)
{
        static struct tm br_time;
        register struct tm *timep = &br_time;
        time_t time = *timer;
        register unsigned long dayclock, dayno;
        int year = EPOCH_YR;

        dayclock = (unsigned long)time % SECS_DAY;
        dayno = (unsigned long)time / SECS_DAY;

        timep->tm_sec = dayclock % 60;
        timep->tm_min = (dayclock % 3600) / 60;
        timep->tm_hour = dayclock / 3600;
        timep->tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */
        while (dayno >= YEARSIZE(year)) {
                dayno -= YEARSIZE(year);
                year++;
        }
        timep->tm_year = year - YEAR0;
        timep->tm_yday = dayno;
        timep->tm_mon = 0;
        while (dayno >= _ytab[LEAPYEAR(year)][timep->tm_mon]) {
                dayno -= _ytab[LEAPYEAR(year)][timep->tm_mon];
                timep->tm_mon++;
        }
        timep->tm_mday = dayno + 1;
        timep->tm_isdst = 0;

        return timep;
}


time_t mktime(struct tm * timeptr){
    time_t rtn = 0;
    rtn+=timeptr->tm_sec;
    rtn+=timeptr->tm_min*60;
    rtn+=(timeptr->tm_hour-1)*60*60;
    rtn+=(timeptr->tm_mday-1)*60*60*24;
    for(int i=0;i<timeptr->tm_mon;i++)
        rtn+=60*60*24*_ytab[LEAPYEAR(YEAR0+timeptr->tm_year)][i];
    for(int y = YEAR0+timeptr->tm_year-1;y>=EPOCH_YR;y--)
        rtn+=60*60*24*YEARSIZE(y);
    return rtn;
}

#endif
