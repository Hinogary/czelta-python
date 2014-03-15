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
using namespace std;
class Station {
public:
    Station();
    short* lastTDCCorrect();
    short* TDCCorrect(time_t time);
    double* detectorPos();
    ~Station();
    inline const char* name(){return _name.c_str();};
    inline int id(){return _ID;};
    static Station& getStation(uint8_t ID);
    static Station& getStation(string name);
    static bool addStation(Station& station);
    inline static bool active(uint8_t index){
        return _actives[index];
    };
    struct TDCCorrection{
        time_t from;
        short tdc[3];
    };
//private:
    static Station _stations[256];
    static bitset<256> _actives;
    static short* null_correction;
    int _ID;
    string _name;
    vector<string> _name_files;
    double _gpsposition[4];
    double _detectorpos[4];
    vector<TDCCorrection> _TDCCorections;
};
#endif	/* STATION_H */
