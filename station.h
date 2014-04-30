/*
 * Author: Martin Quarda
 */
#include "common_func.h"
#include <time.h>

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
    TDCCorrection* corrections;
    int corrections_size;
    int corrections_capacity;
} czelta_station;

extern czelta_station czelta_station_stations[256];
extern short czelta_station_nullCorrection[3];
extern char czelta_station_activate[256];

czelta_station* czelta_station_getStationById(int id);

int czelta_station_exist(int id);
czelta_station* czelta_station_create(int id);
void czelta_station_add(czelta_station *st);
void czelta_station_delete(int id);

void czelta_station_init(czelta_station* st);
void czelta_station_clear(czelta_station* st);

void czelta_station_setName(czelta_station* st, char* name);
void czelta_station_setGPSPosition(czelta_station* st, double latitude, double longitude, double height);
void czelta_station_setDetectorPosition(czelta_station* st, float x1, float y1, float x2, float y2);
void czelta_station_clearTDCCorrect(czelta_station* st);
void czelta_station_pushTDCCorrect(czelta_station* st, time_t from, short tdc0, short tdc1, short tdc2);
double czelta_station_distance_beetwen(czelta_station* st1, czelta_station* st2);

int czelta_station_id(czelta_station* st);
extern char* czelta_station_name(czelta_station* st);
short* czelta_station_lastTDCCorrect(czelta_station* st);
short* czelta_station_TDCCorrect(czelta_station* st, time_t t);
double* czelta_station_GPSPosition(czelta_station* st);
float* czelta_station_detectorPosition(czelta_station* st);
#ifdef __cplusplus
}

#include <string>
#include <vector>

class Station {
public:
    inline Station(){
        czelta_station_init(&st);
    }
    inline Station(int ID){
        czelta_station_init(&st);
        st.id = ID;
    };
    inline ~Station(){
        czelta_station_clear(&st);
    };
    inline short* lastTDCCorrect(){
        return czelta_station_lastTDCCorrect(&st);
    };
    inline short* TDCCorrect(time_t time){
        return czelta_station_TDCCorrect(&st, time);
    };
    inline float* detectorPosition(){
        return czelta_station_detectorPosition(&st);
    };
    //latitude, longtitude, height
    inline double* GPSPosition(){
        return czelta_station_GPSPosition(&st);
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

    inline void setName(char* name){
        czelta_station_setName(&st, name);
    };
    inline void setGPSPosition(double latitude, double longitude, double height){
        czelta_station_setGPSPosition(&st, latitude, longitude, height);
    };
    inline void setDetectorPosition(float x1, float y1, float x2, float y2){
        czelta_station_setDetectorPosition(&st, x1, y1, x2, y2);
    };
    inline void clearTDCCorrect(){
        czelta_station_clearTDCCorrect(&st);
    };
    inline void pushTDCCorrect(time_t from, short tdc0, short tdc1, short tdc2){
        czelta_station_pushTDCCorrect(&st, from, tdc0, tdc1, tdc2);
    };
    inline void pushTDCCorrect(std::string from, short tdc0, short tdc1, short tdc2){
        czelta_station_pushTDCCorrect(&st, string_date(from), tdc0, tdc1, tdc2);
    };
    
    //static methods
    inline static Station& getStation(uint8_t ID){
        return *(Station*)&czelta_station_stations[ID];
    };
    
    inline static Station& getStation(std::string name){
        const char* name_char = name.c_str();
        for(int i=1;i<256;i++){
            if(czelta_station_name(&czelta_station_stations[i]) != NULL && !strcmp(name_char, czelta_station_name(&czelta_station_stations[i])))
                return *(Station*)&czelta_station_stations[i];
        }
        return *(Station*)&czelta_station_stations[0];
    };
    
    inline static std::vector<Station*> getStations(){
        std::vector<Station*> sts;
        for(int i=1;i<256;i++)
            if(czelta_station_stations[i].id == i)
                sts.push_back((Station*)&czelta_station_stations[i]);
        return sts;
    };
    
    //return false if added
    inline static void addStation(Station station){
        czelta_station_add(&station.st);
    };
    
    inline static bool active(uint8_t index){
        return czelta_station_activate[index];
    };
    
private:
    czelta_station st;
};
#endif

#endif	/* STATION_H */
