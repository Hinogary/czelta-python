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

#ifdef _WIN32
//own strptime, light mktime, and gmtime because standart library of windows is ...
#ifndef strptime_h
#define strptime_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <ctype.h>

int strnicmp(const char *pStr1, const char *pStr2, size_t Count)
{
    char c1, c2;
    int v;

    if (Count == 0)
        return 0;

    do {
        c1 = *pStr1++;
        c2 = *pStr2++;
        // the casts are necessary when pStr1 is shorter & char is signed
        v = (unsigned int) tolower(c1) - (unsigned int) tolower(c2);
    } while ((v == 0) && (c1 != '\0') && (--Count > 0));

    return v;
}
#define YEAR0           1900                    /* the first year */
#define EPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SECS_DAY        (24L * 60L * 60L)
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define TIME_MAX        ULONG_MAX
#define ABB_LEN         3
const int _ytab[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
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

const char * strp_weekdays[] =
    { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};
const char * strp_monthnames[] =
    { "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};
bool strp_atoi(const char * & s, int & result, int low, int high, int offset)
    {
    bool worked = false;
    char * end;
    unsigned long num = strtoul(s, & end, 10);
    if (num >= (unsigned long)low && num <= (unsigned long)high)
        {
        result = (int)(num + offset);
        s = end;
        worked = true;
        }
    return worked;
    }
char * strptime(const char *s, const char *format, struct tm *tm)
    {
    bool working = true;
    while (working && *format && *s)
        {
        switch (*format)
            {
        case '%':
            {
            ++format;
            switch (*format)
                {
            case 'a':
            case 'A': // weekday name
                tm->tm_wday = -1;
                working = false;
                for (size_t i = 0; i < 7; ++ i)
                    {
                    size_t len = strlen(strp_weekdays[i]);
                    if (!strnicmp(strp_weekdays[i], s, len))
                        {
                        tm->tm_wday = i;
                        s += len;
                        working = true;
                        break;
                        }
                    else if (!strnicmp(strp_weekdays[i], s, 3))
                        {
                        tm->tm_wday = i;
                        s += 3;
                        working = true;
                        break;
                        }
                    }
                break;
            case 'b':
            case 'B':
            case 'h': // month name
                tm->tm_mon = -1;
                working = false;
                for (size_t i = 0; i < 12; ++ i)
                    {
                    size_t len = strlen(strp_monthnames[i]);
                    if (!strnicmp(strp_monthnames[i], s, len))
                        {
                        tm->tm_mon = i;
                        s += len;
                        working = true;
                        break;
                        }
                    else if (!strnicmp(strp_monthnames[i], s, 3))
                        {
                        tm->tm_mon = i;
                        s += 3;
                        working = true;
                        break;
                        }
                    }
                break;
            case 'd':
            case 'e': // day of month number
                working = strp_atoi(s, tm->tm_mday, 1, 31, -1);
                break;
            case 'D': // %m/%d/%y
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_mon, 1, 12, -1);
                if (working && *s == '/')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_mday, 1, 31, -1);
                    if (working && *s == '/')
                        {
                        ++ s;
                        working = strp_atoi(s, tm->tm_year, 0, 99, 0);
                        if (working && tm->tm_year < 69)
                            tm->tm_year += 100;
                        }
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'H': // hour
                working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
                break;
            case 'I': // hour 12-hour clock
                working = strp_atoi(s, tm->tm_hour, 1, 12, 0);
                break;
            case 'j': // day number of year
                working = strp_atoi(s, tm->tm_yday, 1, 366, -1);
                break;
            case 'm': // month number
                working = strp_atoi(s, tm->tm_mon, 1, 12, -1);
                break;
            case 'M': // minute
                working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                break;
            case 'n': // arbitrary whitespace
            case 't':
                while (isspace((int)*s))
                    ++s;
                break;
            case 'p': // am / pm
                if (!strnicmp(s, "am", 2))
                    { // the hour will be 1 -> 12 maps to 12 am, 1 am .. 11 am, 12 noon 12 pm .. 11 pm
                    if (tm->tm_hour == 12) // 12 am == 00 hours
                        tm->tm_hour = 0;
                    }
                else if (!strnicmp(s, "pm", 2))
                    {
                    if (tm->tm_hour < 12) // 12 pm == 12 hours
                        tm->tm_hour += 12; // 1 pm -> 13 hours, 11 pm -> 23 hours
                    }
                else
                    working = false;
                break;
            case 'r': // 12 hour clock %I:%M:%S %p
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_hour, 1, 12, 0);
                if (working && *s == ':')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                    if (working && *s == ':')
                        {
                        ++ s;
                        working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
                        if (working && isspace((int)*s))
                            {
                            ++ s;
                            while (isspace((int)*s))
                                ++s;
                            if (!strnicmp(s, "am", 2))
                                { // the hour will be 1 -> 12 maps to 12 am, 1 am .. 11 am, 12 noon 12 pm .. 11 pm
                                if (tm->tm_hour == 12) // 12 am == 00 hours
                                    tm->tm_hour = 0;
                                }
                            else if (!strnicmp(s, "pm", 2))
                                {
                                if (tm->tm_hour < 12) // 12 pm == 12 hours
                                    tm->tm_hour += 12; // 1 pm -> 13 hours, 11 pm -> 23 hours
                                }
                            else
                                working = false;
                            }
                        }
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'R': // %H:%M
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
                if (working && *s == ':')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'S': // seconds
                working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
                break;
            case 'T': // %H:%M:%S
                {
                const char * s_save = s;
                working = strp_atoi(s, tm->tm_hour, 0, 23, 0);
                if (working && *s == ':')
                    {
                    ++ s;
                    working = strp_atoi(s, tm->tm_min, 0, 59, 0);
                    if (working && *s == ':')
                        {
                        ++ s;
                        working = strp_atoi(s, tm->tm_sec, 0, 60, 0);
                        }
                    }
                if (!working)
                    s = s_save;
                }
                break;
            case 'w': // weekday number 0->6 sunday->saturday
                working = strp_atoi(s, tm->tm_wday, 0, 6, 0);
                break;
            case 'Y': // year
                working = strp_atoi(s, tm->tm_year, 1900, 65535, -1900);
                break;
            case 'y': // 2-digit year
                working = strp_atoi(s, tm->tm_year, 0, 99, 0);
                if (working && tm->tm_year < 69)
                    tm->tm_year += 100;
                break;
            case '%': // escaped
                if (*s != '%')
                    working = false;
                ++s;
                break;
            default:
                working = false;
                }
            }
            break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\f':
        case '\v':
            // zero or more whitespaces:
            while (isspace((int)*s))
                ++ s;
            break;
        default:
            // match character
            if (*s != *format)
                working = false;
            else
                ++s;
            break;
            }
        ++format;
        }
    return (working?(char *)s:0);
    }
#endif
#endif
