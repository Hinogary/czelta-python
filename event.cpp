/*
 * Author: Martin Quarda
 *
 * Created on 6. listopad 2013, 8:54
 */

#include <string.h>

#include "event.h"
#include "station.h"

Event::Event(){

}

Event::Event(WebEvent e){
    _timestamp = e.timestamp;
    _last_second = e.last_second;
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
    _byte = e.byte&1;
}

Event::Event(time_t timestamp,double last_secod, int16_t TDC0, int16_t TDC1, int16_t TDC2, int16_t ADC0, int16_t ADC1, int16_t ADC2, int16_t t0, int16_t t1, int16_t t2, int8_t t_crate, bool calibration){
    this->_timestamp = timestamp;
    this->_last_second = last_secod;
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
    this->_byte = (calibration?1:0);
}

Event::Event(const Event& orig){
    memcpy(this,&orig,sizeof(Event));
}

/**
 *
 * @param station
 * @return {horizont, azimuth}
 */
array<float,2> Event::calculateDir(Station *st) const{
    short* TDC;// = correctedTDC(st);
    const double t1 = (TDC[1] - TDC[0])*25 * 1e-12;
    const double t2 = (TDC[2] - TDC[0])*25 * 1e-12;
    const double x1 = st->detectorPosition()[0];
    const double y1 = st->detectorPosition()[1];
    const double x2 = st->detectorPosition()[2];
    const double y2 = st->detectorPosition()[3];
    const double c2 = SPEED_OF_LIGHT*SPEED_OF_LIGHT;
    double x = c2 * (t2 * y1 - t1 * y2) / (x1 * y2 - x2 * y1);
    double y = c2 * (t2 * x1 - t1 * x2) / (y1 * x2 - y2 * x1);
    double size = sqrt(x * x + y * y);
    const double a = c2 - x * x - y * y;
    if (a < 0)return array<float,2>{0,0};
    double z = sqrt(a);
    array<float,2> rtn;
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
    if (rtn[0]!=rtn[0] || rtn[1]!=rtn[1])return array<float,2>{0,0};
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
};
