/*
 * Author: Martin Quarda
 */

#ifndef EVENTREADER_H
#define	EVENTREADER_H
#include <functional>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "event.h"
#include "station.h"

struct Overlap{
    int measureTime;
    int normal_events[3];
    int calibration_events[3];
};

 class EventReader {
public:
    EventReader();
    EventReader(const EventReader&);
    inline Event& item(int index){return events[index];};
    inline Event& item(int run, int index){return item(runs[run].beginIndex+index);};
    inline int numberOfEvents(){return events.size();};
    inline int numberOfEvents(int run){return runs[run].endIndex-runs[run].beginIndex;};
    inline int numberOfRuns(){return runs.size();};
    struct Run{
        int beginTimestamp, beginIndex, endTimestamp, endIndex;
    };
    inline Run run(int i){return runs[i];};
    inline int runStartIndex(int i){return runs[i].beginIndex;};
    inline int runStart(int i){return runs[i].beginTimestamp;};
    inline int runEndIndex(int i){return runs[i].endIndex;};
    inline int runEnd(int i){return runs[i].endTimestamp;};
    int measurelength();
    inline int filterCalibs(){if(!_clearedCalibs && (_clearedCalibs=true))return filter(function<bool(Event&)>([](Event& e)->bool{return e.isCalib();}));else return true;};
    inline int filterMaxTDC(){return filter(function<bool(Event&)>([](Event& e)->bool{return e.TDC0()==4095 || e.TDC1()==4095 || e.TDC2()==4095;}));};
    inline int filterMaxADC(){return filter(function<bool(Event&)>([](Event& e)->bool{return e.ADC0()==2047 || e.ADC1()==2047 || e.ADC2()==2047;}));};
    inline int filterMinADC(){return filter(function<bool(Event&)>([](Event& e)->bool{return e.ADC0()==0 || e.ADC1()==0 || e.ADC2()==0;}));};
    inline bool clearedCalibs(){return _clearedCalibs;};
    //return number of events being filtered
    int filter(function<bool(Event&)>);
    int removeInterval(int from, int to);
    inline int filter(bool (*func)(Event&)){return filter(function<bool(Event&)>(func));};
    inline int flux(int from, int to){return firstOlderThan(to)-firstOlderThan(from);};
    int firstOlderThan(int timestamp) const;
    int lastEarlierThan(int timestamp) const;
    ~EventReader();
    static void setFilesDirectory(string dir);
    inline static string getFilesDirectory(){return files_directory;};
    //void checkRuns(int maxDiffbetweenEvents = 0);
    Overlap overlap(EventReader* other, EventReader* other2 = nullptr);
    static uint32_t* fileFromTo(char* filename);
    bool loadDatFile(char* filename);
    bool loadTxtFile(char* filename);
    bool saveDatFile(char* filename);
    bool saveTxtFile(char* filename, bool x_events = true);
    void setStation(uint8_t station);
    inline int getStation(){return _station;};
    inline Station& getRStation(){return Station::getStation(_station);};
private:
#define PART_SIZE (86400/64)
    uint p_start_timestamp;
    uint p_end_timestamp;
    //ocupies about 20 MB per 10 years of data and very much speedup finding events by time -> needed almost by overlaps
    vector<uint> parts_index;
    void clear();
    void addRun(int endIndex = -1);
    bool _clearedCalibs;
    static string files_directory;
    uint8_t _station;
    vector<Event> events;
    vector<Run> runs;
};


#endif	/* EVENTREADER_H */
