/*
 * Author: Martin Quarda
 *
 * Created on 6. listopad 2013, 8:54
 */

#include <string.h>

#include "event.h"
#include "station.h"

WebEvent::WebEvent(){
    
}

WebEvent::WebEvent(const Event& e, bool run){
    timestamp = e.timestamp();
    last_second = e.last_second();
    TDC[0] = e.TDC0();
    TDC[1] = e.TDC1();
    TDC[2] = e.TDC2();
    ADC[0] = e.ADC0();
    ADC[1] = e.ADC1();
    ADC[2] = e.ADC2();
    t[0] = e.t0raw();
    t[1] = e.t1raw();
    t[2] = e.t2raw();
    t_crate = e.tCrateRaw();
    byte = e.isCalib()?1:0 | run?4:0;
}

Event::Event(){

}

Event::Event(WebEvent& e, uint8_t station){
    _timestamp = e.timestamp;
    _last_second = static_cast<uint64_t>(e.last_second*10+0.5);
    _TDC0 = e.TDC[0];
    _TDC1 = e.TDC[1];
    _TDC2 = e.TDC[2];
    _ADC0 = e.ADC[0];
    _ADC1 = e.ADC[1];
    _ADC2 = e.ADC[2];
    _t0 = e.t[0];
    _t1 = e.t[1];
    _t2 = e.t[2];
    _t_crate = e.t_crate;
    _calibration = e.byte&1;
    _station = station;
}

Event::Event(time_t timestamp,double last_second, int16_t TDC0, int16_t TDC1, int16_t TDC2, int16_t ADC0, int16_t ADC1, int16_t ADC2, int16_t t0, int16_t t1, int16_t t2, int8_t t_crate, bool calibration, uint8_t station){
    this->_timestamp = timestamp;
    this->_last_second = static_cast<uint64_t>(last_second*10+0.5);
    this->_TDC0 = TDC0;
    this->_TDC1 = TDC1;
    this->_TDC2 = TDC2;
    this->_ADC0 = ADC0;
    this->_ADC1 = ADC1;
    this->_ADC2 = ADC2;
    this->_t0 = t0;
    this->_t1 = t1;
    this->_t2 = t2;
    this->_t_crate = t_crate;
    this->_calibration = calibration;
    this->_station = station;
}

Event::Event(const Event& orig){
    memcpy(this,&orig,sizeof(Event));
}

/**
 *
 * @param station
 * @return {horizont, azimuth}
 */
float* Event::calculateDir() const{
    static float rtn[2];
    rtn[0] = 0;rtn[1] = 0;
    if(isCalib())return rtn;
    short* TDC = TDCCorrected();
    double *detPos = Station::getStation(getStation()).detectorPosition(); 
    const double t1 = (TDC[1] - TDC[0])*25 * 1e-12;
    const double t2 = (TDC[2] - TDC[0])*25 * 1e-12;
    const double x1 = detPos[0];
    const double y1 = detPos[1];
    const double x2 = detPos[2];
    const double y2 = detPos[3];
    const double c2 = SPEED_OF_LIGHT*SPEED_OF_LIGHT;
    double x = c2 * (t2 * y1 - t1 * y2) / (x1 * y2 - x2 * y1);
    double y = c2 * (t2 * x1 - t1 * x2) / (y1 * x2 - y2 * x1);
    double size = sqrt(x * x + y * y);
    const double a = c2 - x * x - y * y;
    if (a < 0)return rtn;
    double z = sqrt(a);
    rtn[0] = (float) atan(z / size);
    if (x <= 0) {
        if (y <= 0) {
            //III. Kvadrant
            rtn[1] = asin(-x / size);
        } else {
            //II. Kvadrant
            rtn[1] = M_PI - asin(-x / size);
        }
    } else {
        if (y <= 0) {
            //IV. Kvadrant
            rtn[1] = 2 * M_PI - asin(x / size);
        } else {
            //I. Kvadrant
            rtn[1] = M_PI + asin(x / size);
        }
    }
    if (rtn[0]!=rtn[0] || rtn[1]!=rtn[1]){
        rtn[0] = 0;rtn[1] = 0;
        return rtn;
    };
    rtn[0]*=180/M_PI;
    rtn[1]*=180/M_PI;
    return rtn;
}

string Event::toString() const{
    char buffer[90];
    const tm tm = getTime();
    sprintf(buffer,
                "%s %d %02d %02d %02d %02d %02d %.1f %d %d %d %d %d %d %.1f %.1f %.1f %.1f",
                isCalib() ? "c" : "a",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                last_second(), TDC0(), TDC1(), TDC2(), ADC0(), ADC1(), ADC2(), t0(), t1(), t2(), tCrate()
           );
    return string(buffer);
}

short* Event::TDCCorrected() const{
    static short static_TDCCorrected[3];
    short* correction = Station::getStation(getStation()).TDCCorrect(timestamp());
    static_TDCCorrected[0] = TDC0()+correction[0];
    static_TDCCorrected[1] = TDC1()+correction[1];
    static_TDCCorrected[2] = TDC2()+correction[2];
    return static_TDCCorrected;
}

short Event::TDC0Corrected() const{
    return TDC0()+Station::getStation(getStation()).TDCCorrect(timestamp())[0];
}

short Event::TDC1Corrected() const{
    return TDC1()+Station::getStation(getStation()).TDCCorrect(timestamp())[1];
}

short Event::TDC2Corrected() const{
    return TDC2()+Station::getStation(getStation()).TDCCorrect(timestamp())[2];
}
