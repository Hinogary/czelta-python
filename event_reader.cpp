/*
 * File:   EventReader.cpp
 * Author: martin
 *
 * Created on 6. listopad 2013, 9:48
 */

#include "event_reader.h"
#include <time.h>

string EventReader::files_directory = "";
const static string binaryFileHead = "CzeltaDataFile.1";

EventReader::EventReader(){
    _maxDiffbetweenEvents = 0;
}

EventReader::~EventReader() {
    _maxDiffbetweenEvents = 0;
    clear();
}

void EventReader::setFilesDirectory(string dir){
    files_directory = dir;
}

bool EventReader::loadDatFile(char* filename){
    clear();
    _progress = 0;
    ifstream in;
    in.open(filename, ios_base::binary);
    if(!in.is_open()){
        return true;
    }
    in.seekg(0,ifstream::end);
    int length = in.tellg()>>5;
    in.seekg(0,ifstream::beg);
    //check CzeltaDataFile.1
    char buffer[17];
    buffer[16]='\0';
    in.read(buffer,16);
    const string head = "CzeltaDataFile.1";
    if(!(head==buffer)){
        cout<<"Hlavička souboru \""<<filename<<"\" neodpoví­dá \"CzeltaDataFile.1\"."<<endl;
        return true;
    }
    events.resize(length);
    _progress=0.1;
    in.read((char*)events.data(),length<<5);
    in.close();
    loadedFrom = filename;
    _progress=0.9;
    checkRuns();
    _progress = 1;
    return false;
}

bool EventReader::loadTxtFile(char* filename){
    clear();
    bool nextRun = false;
    char line[90];
    char _double[10];
    ifstream in;
    Event e;
    char c;
    int d;
    int year,month,day,hour,minute,second;
    char temp[4][5];
    in.open(filename);
    if(!in.is_open())return true;
    in.seekg(0,ios_base::end);
    int len = in.tellg();
    in.seekg(0,ios_base::beg);
    while(true){
        in.getline(line,90);
        if(in.eof())break;
        if((d = sscanf(line,"%c %d %d %d %d %d %d %s %d %d %d %d %d %d %s %s %s %s",
                &c,&year,&month,&day,&hour,&minute,&second,&_double,
                &e._TDC0, &e._TDC1, &e._TDC2,
                &e._ADC0, &e._ADC1, &e._ADC2,
                &temp[0], &temp[1], &temp[2], &temp[3])
                )!=18 && c!='x'){
            cout<<d<<endl;
            cout<<"špatný formát na řádku "<<(events.size()+runs.size()+1)<<endl;
            cout<<line<<endl;
            clear();
            return true;
        };
        if(c=='x'){
            nextRun = true;
            addRun(events.size()-1);
            continue;
        }
        e._last_second = atof(_double);
        e._timestamp = date(year,month,day,hour,minute,second);
        e._t0 = atof(temp[0])*2;
        e._t1 = atof(temp[1])*2;
        e._t2 = atof(temp[2])*2;
        e._t_crate = atof(temp[3])*2;
        e._byte = (nextRun?4:0)|(c=='c'?1:0);
        events.push_back(*((Event*)&e));
        nextRun = false;
        _progress = double(in.tellg())/len;
    }
    in.close();
    events.shrink_to_fit();
    runs.shrink_to_fit();
    loadedFrom = filename;
    _progress = 1;
    return false;
}

bool EventReader::saveDatFile(char* filename){
    ofstream out;
    out.open(filename,ios::binary);
    if(!out.is_open())return true;
    out.write((char*)events.data(),sizeof(Event)*events.size());
    out.close();
    return false;
}

bool EventReader::saveTxtFile(char* filename){
    ofstream out;
    out.open(filename);
    if(!out.is_open())return true;
    for(uint i=0;i<events.size();i++){
        if(events[i].isRun()){
            out<<"x 0 0 0 0 0 0 0.0 0 0 0 0 0 0 0.0 0.0 0.0 0.0"<<endl;
        }
        out<<events[i]<<endl;
    }
    return false;
}

void EventReader::addRun(int endIndex){
    if(endIndex == 0){
        endIndex = events.size()-1;
    }
    int startIndex = 0;
    if(runs.size()>0)
        startIndex = runs[runs.size()-1].endIndex+1;
    if(startIndex>=endIndex)return;
    runs.push_back(Run{events[startIndex].timestamp(),startIndex,events[endIndex].timestamp(),endIndex});
}

void EventReader::checkRuns(int maxDiffbetweenEvents){
    if(events.size()==0)return;
    if(maxDiffbetweenEvents==0)
        maxDiffbetweenEvents = _maxDiffbetweenEvents;
    else
        _maxDiffbetweenEvents = maxDiffbetweenEvents;
    runs.clear();
    events[0]._byte|=4;
    int beforeTimestamp = events[0].timestamp();
    if(maxDiffbetweenEvents<=0)
        for(uint i=1;i<events.size();i++){
            if(events[i].isRun())
                addRun(i-1);
        }
    else
        for(uint i=1;i<events.size();i++){
            if(events[i].isRun() || (events[i].timestamp()-beforeTimestamp>maxDiffbetweenEvents)){
                addRun(i-1);
                events[i]._byte |= 4;
            }
            beforeTimestamp = events[i].timestamp();
        }
    addRun(events.size()-1);
    runs.shrink_to_fit();
}


int EventReader::filter(function<bool(Event&)> filter_func){
    uint current = 0;
    for(uint i=0;i<events.size();i++){
        if(!filter_func(events[i])){
            events[current]=events[i];
            current++;
        }else if(events[i].isRun() && i+1<events.size()){
            events[i+1]._byte|=4;
        }
    }
    int rtn = events.size()-current;
    events.resize(current);
    events.shrink_to_fit();
    checkRuns();
    return rtn;
}

void EventReader::clear(){
    _clearedCalibs = false;
    _calculeddirs = false;
    events.clear();
    runs.clear();
}

int EventReader::firstOlderThan(int timestamp) const{
    uint i = 0;
    for(i=0;i<runs.size();i++){
        if(runs[i].beginTimestamp>timestamp){
            if(i>0){
                i = runs[i-1].beginIndex;
                break;
            }else
                return 0;
        }
    }
    while(i<events.size() && timestamp>=events[i].timestamp())
        i++;
    return i;
}
int EventReader::lastEarlierThan(int timestamp) const{
    uint i = 0;
    for(i=0;i<runs.size();i++){
        if(runs[i].beginTimestamp>timestamp){
            if(i>0){
                i = runs[i-1].beginIndex;
                break;
            }else
                return 0;
        }
    }
    while(i<events.size() && timestamp>events[i].timestamp())
        i++;
    return --i;
}

int EventReader::measurelength(){
    int len = 0;
    for(int i=0;i<runs.size();i++){
        len+=runs[i].endTimestamp-runs[i].beginTimestamp;
    }
    return len;
}

Overlap EventReader::overlap(EventReader &other){
    Overlap rtn{0,{0,0},{0,0}};
    int from, to, i[] = {0,0};
    while(numberOfRuns()>i[0] && other.numberOfRuns()>i[1]){
        from = max(runStart(i[0]),other.runStart(i[1]));
        to = min(runEnd(i[0]),other.runEnd(i[1]));
        if(to-from>0){
            auto max = lastEarlierThan(to);
            for(int i=firstOlderThan(from);i<=max;i++){
                if(item(i).isCalib())
                    rtn.calibration_events[0]++;
                else
                    rtn.normal_events[0]++;
            }
            max = other.lastEarlierThan(to);
            for(int i=other.firstOlderThan(from);i<=max;i++){
                if(other.item(i).isCalib())
                    rtn.calibration_events[1]++;
                else
                    rtn.normal_events[1]++;
            }
            rtn.measureTime+=to-from;
        };
        if(other.runEnd(i[1])>runEnd(i[0]))
            i[0]++;
        else
            i[1]++;
    }
    return rtn;
}

array<int,2> EventReader::fileFromTo(char* filename){
    ifstream in(filename);
    char line[90];
    array<int,2> rtn = {0,0};
    in.read(line,16);
    if(line[0]=='x' || line[0]=='c' || line[0]=='a'){
        in.seekg(0);
        in.getline(line,90);
        if(line[0]=='x')
            in.getline(line,90);
        int year,month,day,hour,minute,second;
        sscanf(line+2,"%d %d %d %d %d %d",&year,&month,&day,&hour,&minute,&second);
        rtn[0] = date(year,month,day,hour,minute,second);
        in.seekg(-100,ios_base::end);
        in.getline(line,90);
        in.getline(line,90);
        sscanf(line+2,"%d %d %d %d %d %d",&year,&month,&day,&hour,&minute,&second);
        rtn[1] = date(year,month,day,hour,minute,second);
    }else{
        line[16]='\0';
        const string head = "CzeltaDataFile.1";
        if(head!=line){
            return rtn;
        }
        in.read(line,4);
        rtn[0] = *(int*)line;
        in.seekg(-32,ios_base::end);
        in.read(line,4);
        rtn[1] = *(int*)line;
    }
    return rtn;
}
