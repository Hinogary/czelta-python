/*
 * Author: Martin Quarda
 */
#include <string>
#include <vector>
#include <stdlib.h>
#include <bitset>

#include "common_func.h"

#ifndef STATION_H
#define	STATION_H


#ifdef __cplusplus
extern "C"{
#endif
typedef struct{
    time_t from;
    short correction[3];
} TDCCorrection;

typedef struct{
    int id;
    char* name;
    double gps_position[3];
    float detector_pos[4];
    char** file_names;
    int file_names_size;
    int file_names_capacity;
    TDCCorrection* corrections;
    int corrections_size;
    int corrections_capacity;
} czelta_station;

extern czelta_station* czelta_station_stations;
extern short* czelta_station_nullCorrection;
extern bool* czelta_station_activate;

czelta_station* czelta_station_getStationById(int id);

bool czelta_station_exist(int id);
czelta_station* czelta_station_create(int id);
void czelta_station_add(czelta_station *st);
void czelta_station_delete(int id);

void czelta_station_init(czelta_station* st);
void czelta_station_clear(czelta_station* st);

void czelta_station_setName(czelta_station* st, char* name);
void czelta_station_setGPSPosition(czelta_station* st, double latitude, double longitude, double height);
void czelta_station_setDetectorPosition(czelta_station* st, float x1, float y1, float x2, float y2);
void czelta_station_pushFileName(czelta_station* st, char* filename);
void czelta_station_clearTDCCorrect(czelta_station* st);
void czelta_station_pushTDCCorrect(czelta_station* st, time_t from, short tdc0, short tdc1, short tdc2);
double czelta_station_distance_beetwen(czelta_station* st1, czelta_station* st2);

int czelta_station_id(czelta_station* st);
char* czelta_station_name(czelta_station* st);
short* czelta_station_lastTDCCorrect(czelta_station* st);
short* czelta_station_TDCCorrect(czelta_station* st, time_t t);
double* czelta_station_GPSPosition(czelta_station* st);
float* czelta_station_detectorPosition(czelta_station* st);
#ifdef __cplusplus
}

#include <string>

class Station {
public:
    inline Station(int ID){
        czelta_station_init(&st);
        st->id = ID;
    };
    inline ~Station(){czelta_station_clear(&st);};
    inline short* lastTDCCorrect(){
        return czelta_station_lastTDCCorrect(st);
    };
    inline short* TDCCorrect(time_t time){
        return czelta_station_TDCCorrect(&st, time);
    };
    inline float* detectorPosition(){
        return czelta_station_detectorPosition(&st);
    };
    //latitude, longtitude, height
    inline double* GPSPosition(){
        czelta_station_GPSPosition(&st);
    };
    inline const char* name(){
        return czelta_station_name(&st);
    };
    inline int id(){
        return czelta_station_id(&st);
    };
    inline double distanceTo(Station& st){
        return czelta_station_distance_beetwen(&this->st, &st.st);
    };

    void setName(char* name){
        czelta_station_setName(&st, name);
    };
    void setGPSPosition(double latitude, double longitude, double height){
        czelta_station_setGPSPosition(&st, latitude, longitude, height);
    };
    void setDetectorPosition(float x1, float y1, float x2, float y2){
        czelta_station_setDetectorPosition(&st, x1, y1, x2, y2);
    };
    void clearTDCCorrect(){
        czelta_station_clearTDCCorrect(&st);
    };
    void pushTDCCorrect(time_t from, short tdc0, short tdc1, short tdc2){
        czelta_station_pushTDCCorrect(&st, from, tdc0, tdc1, tdc2);
    };
    void pushTDCCorrect(std::string from, short tdc0, short tdc1, short tdc2){
        czelta_station_push_TDCCorrect(&st, string_date(from), tdc0, tdc1, tdc2);
    };
    void pushFileName(string name){
        czelta_station_pushFileName(&st, name.c_str());
    };
    
    //static methods
    static Station& getStation(uint8_t ID){
        return *(Station*)&czelta_station_stations[ID];
    };
    
    static Station& getStation(string name){
        
    };
    
    static vector<Station*> getStations(){
        
    };
    
    //return false if added
    inline static bool addStation(Station station){
        czelta_station_add(&st);
    };
    
    inline static bool active(uint8_t index){
        return czelta_station_activate[index];
    };
    
private:
    czelta_station st;
};
#endif

#endif	/* STATION_H */
