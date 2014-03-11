/*
 * File:   station.h
 * Author: martin
 *
 * Created on 5. listopad 2013, 20:35
 */
#include <string>
#include <vector>
#include <stdlib.h>
#include <bitset>
#include <tuple>

#include "common_func.h"
#include "event.h"

#ifndef STATION_H
#define	STATION_H
class Station;
using namespace std;
class Station {
public:
    Station();
    short* lastCorrection();
    short* TDCCorrection(time_t time);
    double* detectorPos();
    ~Station();
    inline string name(){
        return _name;
    }
    static Station *getStation(uint8_t ID);
    static Station *getStation(string name);
    inline static bool active(uint8_t index){
        return _actives[index];
    }
//private:
    static Station _stations[256];
    static bitset<256> _actives;
    static array<short,3> null_correction;
    short _ID;
    string _name;
    vector<string> _name_files;
    double _position[4];
    double _detectorPos[4];
    vector<tuple<time_t,short,short,short>> _TDCCorection;
};
#endif	/* STATION_H */
