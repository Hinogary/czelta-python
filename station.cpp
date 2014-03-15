/*
 * File:   station.cpp
 * Author: martin
 *
 * Created on 5. listopad 2013, 20:35
 */

#include "station.h"

Station Station::_stations[256];
bitset<256> Station::_actives;
short* Station::null_correction = new short[3]{0,0,0};

Station::Station() {
    _ID = 0;
    for(int i=0;i<4;i++){
        _gpsposition[i]=0;
        _detectorpos[i]=0;
    }
}
Station::~Station() {

}

Station& Station::getStation(uint8_t ID){
    return _stations[ID];
}

Station& Station::getStation(string name){
    for(uint i=0;i<256;i++){
        if(active(i) && name==getStation(i).name()) return _stations[i];
    }
    return _stations[0];
}

bool Station::addStation(Station& station){
    if(station.id()<1 || station.id()>255 || active(station.id()))return false;
    _actives[station.id()] = true;
    _stations[station.id()] = station;
}

short* Station::lastTDCCorrect(){
    int i = _TDCCorections.size()-1;
    if(i==-1)return &null_correction[0];
    else return _TDCCorections.back().tdc;
}

short* Station::TDCCorrect(time_t time){
    int r = -1;
    for(int i=_TDCCorections.size()-1;i>=0;i--){
        if(_TDCCorections[i].from<time)
            r=i;
        else break;
    }
    if(r>=0)
        return _TDCCorections[r].tdc;
    else
        return null_correction;
}

double* Station::detectorPos(){
    return _detectorpos;
}
