/* 
 * File:   Event.h
 * Author: martin
 *
 * Created on 6. listopad 2013, 8:54
 */

#ifndef EVENT_H
#define	EVENT_H
#ifndef M_PI
    #define M_PI 3.14159265359
typedef unsigned int uint;
#endif
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <array>
#include <stdio.h>
#include "station.h"
#define SPEED_OF_LIGHT 299792458.0
using namespace std;

#pragma pack(1)
class Event {
    friend class EventReader;
public:
    Event();
    Event(int32_t timestamp,double last_secod, int16_t TDC0, int16_t TDC1, int16_t TDC2, int16_t ADC0, int16_t ADC1, int16_t ADC2, int16_t t0, int16_t t1, int16_t t2, int8_t tCrateRaw, uint8_t byte);
    Event(const Event& orig);
    inline int timestamp() const{return _timestamp;};
    inline double last_second() const{return _last_second;};
    inline int64_t tenthOfNSTimestamp() const{return int64_t(timestamp())*10000000000L+int64_t(last_second()*10);};
    inline short* TDC() const{static short static_TDC[3] = {_TDC[0],_TDC[1],_TDC[2]};return static_TDC;};
    array<short,3> correctedTDC (Station* st)const;
    inline array<short,3> correctedTDC(int st) const{return correctedTDC(Station::getStation(st));};
    inline short TDC0() const{return _TDC[0];};
    inline short TDC1() const{return _TDC[1];};
    inline short TDC2() const{return _TDC[2];};
    inline short TDC0corrected(Station *st) const{return correctedTDC(st)[0];};
    inline short TDC1corrected(Station *st) const{return correctedTDC(st)[1];};
    inline short TDC2corrected(Station *st) const{return correctedTDC(st)[2];};
    inline short* ADC() const{static short static_ADC[3] = {_ADC[0],_ADC[1],_ADC[2]}; return static_ADC;};
    inline short ADC0() const{return _ADC[0];};
    inline short ADC1() const{return _ADC[1];};
    inline short ADC2() const{return _ADC[2];};
    inline short* tempsRaw() const{static short static_raw_temps[4] = {_t[0],_t[1],_t[2],_t_crate}; return static_raw_temps;};
    inline float* temps() const{static float static_temps[4] = {_t[0]/2.0f,_t[1]/2.0f,_t[2]/2.0f,_t_crate/2.0f}; return static_temps;};
    inline short t0raw() const{return _t[0];};
    inline short t1raw() const{return _t[1];};
    inline short t2raw() const{return _t[2];};
    inline float t0() const{return _t[0]/2.0f;};
    inline float t1() const{return _t[1]/2.0f;};
    inline float t2() const{return _t[2]/2.0f;};
    inline short tCrateRaw() const{return _t_crate;};
    inline float tCrate() const{return _t_crate/2.0f;};
    inline bool isCalib() const{return _byte&1;};
    inline bool isRun() const{return _byte&4;};
    inline char byte() const{return _byte;};
    inline tm getTime() const{time_t tm = _timestamp;return *gmtime(&tm);};
    inline array<float,2> calculateDir(int station) const{return calculateDir(Station::getStation(station));};
    array<float,2> calculateDir(Station *st) const;
    string toString() const;
private:
    int32_t _timestamp; 
    double_t _last_second;
    int16_t _TDC[3];
    int16_t _ADC[3];
    int16_t _t[3];
    int8_t _t_crate;
    uint8_t _byte;
};
inline ostream& operator << (ostream& os, const Event& e){os<<e.toString();return os;}
#pragma pack(0)
#endif	/* EVENT_H */
