/*
 * Author: Martin Quarda
 *
 * Created on 6. listopad 2013, 9:48
 */

#ifndef EVENTREADER_H
#define	EVENTREADER_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include "event.h"
#include "station.h"

struct Overlap{
    int measureTime;
    int normal_events[2];
    int calibration_events[2];
};

 class EventReader {
public:
    EventReader();
    EventReader(const EventReader&) = delete;
    void setStation(uint8_t);
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
    inline double progress(){return _progress;};
    inline bool clearedCalibs(){return _clearedCalibs;};
    //return number of events being filtered
    int filter(function<bool(Event&)>);
    int firstOlderThan(int timestamp) const;
    int lastEarlierThan(int timestamp) const;
    ~EventReader();
    static void setFilesDirectory(string dir);
    //max distance beetween events in seconds with adding custom run, 0 = withoutadding
    void checkRuns(int maxDiffbetweenEvents = 0);
    Overlap overlap(EventReader &other);
    const static string binaryFileHead;
    static array<int,2> fileFromTo(char* filename);
    bool loadDatFile(char* filename);
    bool loadTxtFile(char* filename);
    bool saveDatFile(char* filename);
    bool saveTxtFile(char* filename);
private:
    void clear();
    double _progress;
    void addRun(int endIndex = 0);
    bool _clearedCalibs;
    bool _calculeddirs;
    int _maxDiffbetweenEvents;
    char* loadedFrom;
    static string files_directory;
    vector<Event> events;
    vector<Run> runs;
};


#endif	/* EVENTREADER_H */

