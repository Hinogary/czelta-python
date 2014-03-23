/*
 * Author: Martin Quarda
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
    //after create have to be added with addStation(), if you want use it with events
    Station(int ID);
    short* lastTDCCorrect();
    short* TDCCorrect(time_t time);
    double* detectorPosition();
    double* GPSPosition();
    ~Station();
    inline const char* name(){return _name.c_str();};
    inline int id(){return _ID;};
    double distanceTo(Station& st);

    void setName(char* name);
    void setGPSPosition(double latitude, double longitude, double height);
    void setDetectorPosition(double x1, double y1, double x2, double y2);
    void clearTDCCorrect(int capacity=1);
    void pushTDCCorrect(time_t from, short tdc0, short tdc1, short tdc2);
    void pushTDCCorrect(string from, short tdc0, short tdc1, short tdc2);
    void pushFileName(string name);
    
    //static methods
    static Station& getStation(uint8_t ID);
    static Station& getStation(string name);
    static vector<Station*> getStations();
    //return false if added
    static bool addStation(Station station);
    inline static bool active(uint8_t index){
        return _actives[index];
    };
    struct TDCCorrection{
        time_t from;
        short tdc[3];
    };
private:
    static Station _stations[256];
    static bitset<256> _actives;
    static short* null_correction;
    int _ID;
    string _name;
    vector<string> _file_names;
    double _gpsposition[3];
    double _detectorpos[4];
    vector<TDCCorrection> _TDCCorections;
};
#endif	/* STATION_H */
