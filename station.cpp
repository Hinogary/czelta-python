/*
 * File:   station.cpp
 * Author: martin
 *
 * Created on 5. listopad 2013, 20:35
 */

#include "station.h"
#include <climits>

Station Station::_stations[256];
bitset<256> Station::_actives;
array<short,3> Station::null_correction = {0,0,0};

Station::Station() {
    _ID = 0;
    for(int i=0;i<4;i++){
        _detectorPos[i]=0;
        _position[i]=0;
    }
}
Station::~Station() {

}

Station* Station::getStation(uint8_t ID){
    return &_stations[ID];
}

Station* Station::getStation(string name){
    for(uint i=0;i<256;i++){
        if(active(i) && name==getStation(i)->name()) return &_stations[i];
    }
    return NULL;
}

short* Station::lastCorrection(){
    int i = _TDCCorection.size()-1;
    if(i==-1)return &null_correction[0];
    else return &get<1>(_TDCCorection[i]);
}

short* Station::TDCCorrection(time_t time){
    int r = INT_MAX;
    for(int i=_TDCCorection.size()-1;i>=0;i--){
        if(get<0>(_TDCCorection[i])<time)
            r=i;
        else break;
    }
    return &get<1>(_TDCCorection[r]);
}

double* Station::detectorPos(){
    return _detectorPos;
}
