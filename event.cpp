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
    byte = (e.isCalib()?1:0) | (run?4:0);
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

bool Event::operator==(const Event& other) const{
    return (_timestamp == other._timestamp
        // _last_second have sometimes error in last digit
        // because of different treating by different platforms
        // for double (double -> long int and back)
        // precision is enought, that it is not important
        && labs(_last_second - other._last_second) < 10
        && _calibration == other._calibration
        && _TDC0 == other._TDC0
        && _TDC1 == other._TDC1
        && _TDC2 == other._TDC2
        && _ADC0 == other._ADC0
        && _ADC1 == other._ADC1
        && _ADC2 == other._ADC2
        && _t0 == other._t0
        && _t1 == other._t1
        && _t2 == other._t2
        && _t_crate == other._t_crate);
        // station not sure if it should be compared, because user sets it and others values should be already sufficient to unique identify event
}

double* Event::directionVector() const{
    static double vector[3];

    short* TDC = TDCCorrected();
    float *detPos = getRStation().detectorPosition();
    const auto c2 = (SPEED_OF_LIGHT*SPEED_OF_LIGHT);
    const auto x1 = detPos[0];
    const auto y1 = detPos[1];
    const auto x2 = detPos[2];
    const auto y2 = detPos[3];
    const short t1 = TDC[1] - TDC[0];
    const short t2 = TDC[2] - TDC[0];

    // just aliases
    auto& vec_x = vector[0];
    auto& vec_y = vector[1];
    auto& vec_z = vector[2];

    vec_x = c2*25*1e-12*(t2*y1 - t1*y2)/
                    (x1*y2 - x2*y1);
    vec_y = c2*25*1e-12*(t2*x1 - t1*x2)/
                    (y1*x2 - y2*x1);
    //vec_z squared
    vec_z = c2 - vec_x*vec_x - vec_y*vec_y;

    //can't determine direction -> TDC is out of view scope
    if (vec_z < 0){
        return nullptr;
    }
    vec_z = sqrt(vec_z);

    return vector;
}

/**
 *
 * @param station
 * @return {azimuth, horizon}
 */
float* Event::calculateDirRadians() const{
    if(isCalib())return nullptr;
    double* vector = directionVector();
    if(vector==nullptr)return nullptr;
    return dirVectorToAh(vector);
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
