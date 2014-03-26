/*
 * Author: Martin Quarda
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

#pragma pack(push, 1)
class Event;
struct WebEvent {
    //don't change anything, this is CzeltaDataFile.1 format of showers
    WebEvent();
    WebEvent(const Event& e, bool run);
    int32_t timestamp; 
    double last_second;
    int16_t TDC[3];
    int16_t ADC[3];
    int16_t t[3];
    int8_t t_crate;
    uint8_t byte;
};
class Event {
    friend class EventReader;
public:
    Event();
    Event(WebEvent& e, uint8_t station);
    Event(time_t timestamp,double last_secod, int16_t TDC0, int16_t TDC1, int16_t TDC2, int16_t ADC0, int16_t ADC1, int16_t ADC2, int16_t t0, int16_t t1, int16_t t2, int8_t tCrateRaw, bool calibration, uint8_t station);
    Event(const Event& orig);
    inline int timestamp() const{return _timestamp;};
    inline double last_second() const{return _last_second;}; //time since last second, in nanoseconds
    inline double time_since_second() const{return _last_second*1e-9;};//time since last second, in seconds
    inline int64_t tenthOfNSTimestamp() const{return int64_t(timestamp())*10000000000L + int64_t(last_second()*10);};
    inline short TDC0() const{return _TDC0;};
    inline short TDC1() const{return _TDC1;};
    inline short TDC2() const{return _TDC2;};
    inline short* TDC() const{
        static short static_TDC[3];
        static_TDC[0]=TDC0(); static_TDC[1]=TDC1(); static_TDC[2]=TDC2();
        return static_TDC;};
    short* TDCCorrected() const;
    short TDC0Corrected() const;
    short TDC1Corrected() const;
    short TDC2Corrected() const;
    inline short ADC0() const{return _ADC0;};
    inline short ADC1() const{return _ADC1;};
    inline short ADC2() const{return _ADC2;};
    short* ADC() const{
        static short static_ADC[3];
        static_ADC[0]=ADC0(); static_ADC[1]=ADC1(); static_ADC[2]=ADC2();
        return static_ADC;};
    inline short t0raw() const{return _t0;};
    inline short t1raw() const{return _t1;};
    inline short t2raw() const{return _t2;};
    inline float t0() const{return t0raw()/2.0f;};
    inline float t1() const{return t1raw()/2.0f;};
    inline float t2() const{return t2raw()/2.0f;};
    inline float tCrate() const{return _t_crate/2.0f;};
    inline short tCrateRaw() const{return _t_crate;};
    inline short* tempsRaw() const{
        static short static_raw_temps[4];
        static_raw_temps[0]=t0raw(); static_raw_temps[1]=t1raw();
        static_raw_temps[2]=t2raw(); static_raw_temps[3]=tCrateRaw();
        return static_raw_temps;};
    inline float* temps() const{
        static float static_temps[4];
        static_temps[0]=t0(); static_temps[1]=t1();
        static_temps[2]=t2(); static_temps[3]=tCrate();
        return static_temps;};
    inline bool isRun() const{return _byte&4;};
    inline bool isCalib() const{return _byte&1;};
    inline tm getTime() const{time_t tm = _timestamp;return *gmtime(&tm);};
    float* calculateDir() const;
    string toString() const;
    inline void setStation(uint8_t st){_station=st;};
private:
    uint32_t _timestamp; 
    double _last_second;
    uint16_t _TDC0, _TDC1, _TDC2;
    uint16_t _ADC0, _ADC1, _ADC2;
    int8_t _t0, _t1, _t2;
    int8_t _t_crate;
    uint8_t _byte;
    uint8_t _station;
};
inline ostream& operator << (ostream& os, const Event& e){os<<e.toString();return os;}
#pragma pack(pop)
#endif	/* EVENT_H */
