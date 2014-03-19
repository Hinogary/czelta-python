/*
 * Author: Martin Quarda
 */

#include "station.h"
#include <stdexcept>
#include "math.h"

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

Station::Station(int id) {
    _ID = id;
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

vector<Station*> Station::getStations(){
    vector<Station*> stations;
    for(int i=0;i<256;i++){
        if(_stations[i].id()!=0)
            stations.push_back(&_stations[i]);
    }
    stations.shrink_to_fit();
    return stations;
}

bool Station::addStation(Station station){
    if(station.id()<1 || station.id()>255 || active(station.id()))return true;
    _actives[station.id()] = true;
    _stations[station.id()] = station;
    return false;
}

short* Station::lastTDCCorrect(){
    if(_TDCCorections.size()==0)return null_correction;
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

void Station::setGPSPosition(double latitude, double longitude, double height){
    _gpsposition[0] = latitude;
    _gpsposition[1] = longitude;
    _gpsposition[2] = height;
}

void Station::setDetectorPosition(double x1, double y1, double x2, double y2){
    _detectorpos[0] = x1;
    _detectorpos[1] = y1;
    _detectorpos[2] = x2;
    _detectorpos[3] = y2;
}

double* Station::detectorPosition(){
    return _detectorpos;
}

void Station::setName(char* name){
    _name = name;
}

double Station::distanceTo(Station& st){
    if(this->id()==0 || st.id()==0)return 0;
   //haversine method
    double dlong = (st.GPSPosition()[1] - this->GPSPosition()[1]) * M_PI / 180.0;
    double dlat = (st.GPSPosition()[0] - this->GPSPosition()[0]) * M_PI / 180.0;
    double a = pow(sin(dlat/2.0), 2) + cos(st.GPSPosition()[0] * M_PI / 180.0) * cos(this->GPSPosition()[0] * M_PI / 180.0) * pow(sin(dlong/2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return 6367 * c;
}

double* Station::GPSPosition(){
    return _gpsposition;
}

void Station::clearTDCCorrect(int capacity){
    _TDCCorections.clear();
    _TDCCorections.reserve(capacity);
}

void Station::pushTDCCorrect(time_t from, short tdc0, short tdc1, short tdc2){
    _TDCCorections.push_back(TDCCorrection{from, tdc0, tdc1, tdc2});
}

void Station::pushTDCCorrect(string from, short tdc0, short tdc1, short tdc2){
    pushTDCCorrect(date(from), tdc0, tdc1, tdc2);
}

void Station::pushFileName(string name){
    _file_names.push_back(name);
}
