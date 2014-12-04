/*
 * Author: Martin Quarda
 */
#include <math.h>
#include "station.h"

czelta_station czelta_station_stations[256];
#define stations czelta_station_stations

short czelta_station_nullCorrection[3] = {0,0,0};
#define nullCorrection czelta_station_nullCorrection

char czelta_station_activate[256];
#define activate czelta_station_activate


czelta_station* czelta_station_getStationById(int id){
    if(stations[id].id == id)
        return &stations[id];
    else
        return stations;
};

int czelta_station_exist(int id){
    return id>0 && id<256 && stations[id].id == id && activate[id];
}

czelta_station* czelta_station_create(int id){
    if(czelta_station_exist(id))
        czelta_station_delete(id);
    czelta_station_init(&stations[id]);
    stations[id].id = id;
    activate[id] = 1;
    return &stations[id];
}

void czelta_station_add(czelta_station *st){
    czelta_station_delete(st->id);
    memcpy(&stations[st->id], st, sizeof(czelta_station));
    memset(st, 0, sizeof(czelta_station));
}

void czelta_station_delete(int id){
    if(czelta_station_exist(id))
        czelta_station_clear(&stations[id]);
    activate[id] = 0;
}

void czelta_station_init(czelta_station* st){
    int i;
    st->name = NULL;
    for(i=0;i<3;i++)
        st->gps_position[i] = 0;
    for(i=0;i<4;i++)
        st->detector_pos[i] = 0;
    st->corrections = NULL;
    st->corrections_size = 0;
    st->corrections_capacity = 0;
}

void czelta_station_clear(czelta_station* st){
    int i;
    if(st->name != NULL)
        free(st->name);
    czelta_station_clearTDCCorrect(st);
}

void czelta_station_setName(czelta_station* st, char* name){
    if(st->name != NULL)
        free(st->name);
    int len = strlen(name);
    st->name = malloc(len+1);
    if(st->name != NULL)
        memcpy(st->name, name, len);
    st->name[len] = '\0';
}

void czelta_station_setGPSPosition(czelta_station* st, double latitude, double longitude, double height){
    st->gps_position[0] = latitude;
    st->gps_position[1] = longitude;
    st->gps_position[2] = height;
}

void czelta_station_setDetectorPosition(czelta_station* st, float x1, float y1, float x2, float y2){
    st->detector_pos[0] = x1;
    st->detector_pos[1] = y1;
    st->detector_pos[2] = x2;
    st->detector_pos[3] = y2;
}

void czelta_station_clearTDCCorrect(czelta_station* st){
    if(st->corrections != NULL)
        free(st->corrections);
    st->corrections = NULL;
    st->corrections_capacity = 0;
    st->corrections_size = 0;
}

void czelta_station_pushTDCCorrect(czelta_station* st, time_t _from, short tdc0, short tdc1, short tdc2){
    if(st->corrections == NULL || st->corrections_capacity <= st->corrections_size){
        TDCCorrection* new_ptr;
        int new_capacity = st->corrections_capacity*2;
        new_capacity = new_capacity==0?5:new_capacity;
        new_ptr = realloc(st->corrections, sizeof(TDCCorrection)*new_capacity);
        if(new_ptr == NULL)
            return;
        st->corrections_capacity = new_capacity;
        st->corrections = new_ptr;
    }
    TDCCorrection c = {_from, {tdc0, tdc1, tdc2}};
    st->corrections[st->corrections_size++] = c;
}

double czelta_station_distance_beetwen(czelta_station* st1, czelta_station* st2){
    return deltaDistance(st1->gps_position, st2->gps_position);
}

int czelta_station_id(czelta_station* st){
    return st->id;
}

char* czelta_station_name(czelta_station* st){
    return st->name;
}

short* czelta_station_lastTDCCorrect(czelta_station* st){
    return st->corrections[st->corrections_size-1].correction;
}

short* czelta_station_TDCCorrect(czelta_station* st, time_t t){
    int r = st->corrections_size - 1;
    int i = r;
    for(;i>=0;i--){
        if(st->corrections[i]._from>time)
            r=i-1;
        else
            break;
    }
    if(r>=0)
        return st->corrections[r].correction;
    else
        return nullCorrection;
}

double* czelta_station_GPSPosition(czelta_station* st){
    return st->gps_position;
}

float* czelta_station_detectorPosition(czelta_station* st){
    return st->detector_pos;
}
#undef nullCorrection
#undef stations
#undef activate
