/*
 * Author: Martin Quarda
 */

#include "station.h"
#include "math.h"
#include "station.h"

#define stations czelta_station_stations
czelta_station stations[256];
#define nullCorrection czelta_station_nullCorrection
short* nullCorrection[3] = {0,0,0};
#define activate czelta_station_activate
bool activate[256];

czelta_station* czelta_station_getStationById(int id){
    if(stations[id].id == id)
        return &stations[id];
    else
        return stations;
};

bool czelta_station_exist(int id){
    return id>0 && id<256 && stations[id].id == id && activate[id];
}

czelta_station* czelta_station_create(int id){
    if(czelta_station_exist(&stations[id]))
        czelta_station_clear(id);
    czelta_station_init(&stations[id]);
    stations[id].id = id;
    activate[id] = true;
}

void czelta_station_add(czelta_station *st){
    czelta_station_delete(st->id);
    memcpy(&stations[st->id], st, sizeof(czelta_station));
    memset(st, 0, sizeof(czelta_station));
}

void czelta_station_delete(int id){
    if(czelta_station_exist(id))
        czelta_station_clear(&stations[id])
    active[id] = false;
}

void czelta_station_init(czelta_station* st){
    st->name = NULL;
    for(int i=0;i<3;i++)
        st->gps_position[i] = 0;
    for(int i=0;i<4;i++)
        st->detector_pos[i] = 0;
    st->file_names_capacity = 5;
    st->file_names_size = 0;
    st->file_names = malloc(st->file_names_capacity*sizeof(void*));
    st->corrections = NULL;
    st->corrections_size = 0;
    st->corrections_capacity = 0;
}

void czelta_station_clear(czelta_station* st){
    if(st->name != NULL)
        free(st->name);
    for(int i=0;i<st->file_names_size;i++)
        free(st->file_names[i])
    free(st->file_names);
    czelta_station_clearTDCCorrect(st);
}

void czelta_station_setName(czelta_station* st, char* name){
    if(st->name != NULL)
        free(st->name);
    int len = strlen(name);
    st->name = malloc(len);
    if(st->name != NULL)
        memcpy(st->name, name, len);
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

void czelta_station_pushFileName(czelta_station* st, char* filename){
    if(st->file_names_size >= st->file_names_capacity){
        char* new_ptr = realloc(st->file_names, st->file_names_capacity*2);
        if(new_ptr == NULL)
            return;
        st->file_names = new_ptr;
        st->file_names_capacity*=2;
    }
    int len = strlen(filename);
    char* copied_filename = malloc(len);
    if(copied_filename == NULL)
        return;
    memcpy(coped_filename, filename, len);
    st->file_names[st->file_names_size++] = copied_filename;
}

void czelta_station_clearTDCCorrect(czelta_station* st){
    if(st->corrections != NULL)
        free(st->corrections);
    st->corrections = NULL;
    st->corrections_capacity = 0;
    st->corrections_size = 0;
}

void czelta_station_pushTDCCorrect(czelta_station* st, time_t from, short tdc0, short tdc1, short tdc2){
    if(st->correction == NULL || st->correction_capacity <= st->correction_size){
        int new_capacity = st->correction_capacity*2;
        new_capacity = new_capacity==0?5:new_capacity;
        TDCCorrection* new_ptr = realloc(st->correction, sizeof(TDCCorrection)*new_capacity);
        if(new_ptr == NULL)
            return;
        st->corrections = new_ptr;
    }
    TDCCorrection c = {from, {tdc0, tdc1, tdc2}};
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
    for(int i=r;i>=0;i--){
        if(st->corrections[i].from>time)
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
