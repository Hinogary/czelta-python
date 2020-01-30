/*
 * Author: Martin Quarda
 */

#include "event_reader.h"
#include <time.h>

string EventReader::files_directory = "";

EventReader::EventReader(){
    _station = 0;
}

EventReader::~EventReader() {
    clear();
}

EventReader::EventReader(const EventReader& er){
    events = er.events;
    p_start_timestamp = er.p_start_timestamp;
    p_end_timestamp = er.p_end_timestamp;
    parts_index = er.parts_index;
    _clearedCalibs = er._clearedCalibs;
    _station = er._station;
    runs = runs;
}

void EventReader::setFilesDirectory(string dir){
    files_directory = dir;
}

bool EventReader::loadDatFile(char* filename){
    clear();
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
    static_assert(sizeof(WebEvent) >= sizeof(Event),"Event cannot be bigger than WebEvent");
    events.reserve(length*sizeof(WebEvent)/sizeof(Event)+1);
    WebEvent* wevents = (WebEvent*)events.data();
    in.read((char*)events.data(),length<<5);
    in.close();
    p_start_timestamp = wevents[0].timestamp/PART_SIZE*PART_SIZE;
    for(int i=0;i<length;i++){
        if(wevents[i].byte&4)
            addRun(i);
        events.push_back(Event(wevents[i],_station));
        while(p_start_timestamp+parts_index.size()*PART_SIZE < events.back().timestamp())
            parts_index.push_back(i);
    }
    p_end_timestamp = events.back().timestamp()/PART_SIZE*PART_SIZE + (events.back().timestamp()%PART_SIZE==0?0:PART_SIZE);
    addRun();
    events.shrink_to_fit();
    parts_index.shrink_to_fit();
    return false;
}

bool EventReader::loadTxtFile(char* filename){
    clear();
    char line[90];
    char _double[10];
    ifstream in;
    char c;
    int d;
    int year,month,day,hour,minute,second, TDC0, TDC1, TDC2, ADC0, ADC1, ADC2;
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
                &TDC0, &TDC1, &TDC2,
                &ADC0, &ADC1, &ADC2,
                &temp[0], &temp[1], &temp[2], &temp[3])
                )!=18 && c!='x'){
            cout<<d<<endl;
            cout<<"špatný formát na řádku "<<(events.size()+runs.size()+1)<<endl;
            cout<<line<<endl;
            clear();
            return true;
        };
        if(c=='x'){
            addRun();
            continue;
        }
        events.push_back(Event(date(year,month,day,hour,minute,second),
            atof(_double),
            TDC0, TDC1, TDC2,
            ADC0, ADC1, ADC2,
            (int)(atof(temp[0])*2), (int)(atof(temp[1])*2), (int)(atof(temp[2])*2), (int)(atof(temp[3])*2),
            c=='c'?true:false,_station));
        if(events.size()==1){
            p_start_timestamp = events[0].timestamp()/PART_SIZE*PART_SIZE;
        }
        while(p_start_timestamp+parts_index.size()*PART_SIZE < events.back().timestamp())
            parts_index.push_back(events.size()-1);
    }
    p_end_timestamp = events.back().timestamp()/PART_SIZE*PART_SIZE + (events.back().timestamp()%PART_SIZE==0?0:PART_SIZE);
    addRun();
    in.close();
    events.shrink_to_fit();
    parts_index.shrink_to_fit();
    runs.shrink_to_fit();
    return false;
}

bool EventReader::saveDatFile(char* filename){
    ofstream out;
    out.open(filename,ios::binary);
    if(!out.is_open())return true;
    const string head = "CzeltaDataFile.1";
    out.write(head.c_str(), 16);
    WebEvent* wevents = new WebEvent[(1<<20)>numberOfEvents()?numberOfEvents():(1<<20)];
    bool is_run = false;
    int run_id = 0;
    for(int i=0, chunk_size = 0;i<numberOfEvents();i+=(1<<20)){
        chunk_size = numberOfEvents()-i;
        chunk_size = chunk_size>(1<<20)?(1<<20):chunk_size;

        for(int j=0;j<chunk_size;j++){
            if(run(run_id).beginIndex == i+j){
                is_run = true;
                ++run_id;
            }else
                is_run = false;
            wevents[j] = WebEvent(item(i+j),is_run);
        }
        out.write((char*)wevents, sizeof(WebEvent)*chunk_size);
    }
    delete[] wevents;
    out.close();
    return false;
}

bool EventReader::saveTxtFile(char* filename, bool x_events){
    ofstream out;
    out.open(filename);
    if(!out.is_open())return true;
    int next_run = 0;
    for(uint i=0;i<events.size();i++){
        if(run(next_run).beginIndex == i && x_events){
            next_run+=1;
            out<<"x 0 0 0 0 0 0 0.0 0 0 0 0 0 0 0.0 0.0 0.0 0.0"<<endl;
        }
        out<<events[i]<<endl;
    }
    return false;
}

void EventReader::addRun(int endIndex){
    if(endIndex == -1){
        endIndex = events.size();
    }
    int startIndex = 0;
    if(runs.size()>0)
        startIndex = runs[runs.size()-1].endIndex;
    if(startIndex>=endIndex){
        return;
    }
    runs.push_back(Run{events[startIndex].timestamp(),startIndex,events[endIndex-1].timestamp(),endIndex});
}

int EventReader::filter(function<bool(Event&)> filter_func){
    uint current = 0;
    auto old_runs = runs;
    runs.clear();
    int current_run_end = old_runs[0].endIndex;
    parts_index.clear();
    for(uint i=0, run_id = 0;i<events.size();i++){
        if(i==current_run_end){
            addRun(current);
            current_run_end = old_runs[++run_id].endIndex;
        }
        if(!filter_func(events[i])){
            events[current++]=events[i];
            if(current==1)
                p_start_timestamp = events[0].timestamp()/PART_SIZE*PART_SIZE;
            while(p_start_timestamp+parts_index.size()*PART_SIZE < events[current-1].timestamp())
                parts_index.push_back(current-1);
        }
    }
    p_end_timestamp = events.back().timestamp()/PART_SIZE*PART_SIZE + (events.back().timestamp()%PART_SIZE==0?0:PART_SIZE);
    int rtn = events.size()-current;
    if(rtn>0){
        events.resize(current);
        events.shrink_to_fit();
    }
    addRun();
    runs.shrink_to_fit();
    parts_index.shrink_to_fit();
    return rtn;
}

void EventReader::clear(){
    _clearedCalibs = false;
    events.clear();
    runs.clear();
    parts_index.clear();
}

int EventReader::firstOlderThan(int timestamp) const{
    uint i = 0;
    if(timestamp>p_end_timestamp)
        i = events.size()-1;
    else if(timestamp<p_start_timestamp)
        i = 0;
    else
        i = parts_index[(timestamp-p_start_timestamp)/PART_SIZE];
    while(i<events.size() && timestamp>=events[i].timestamp())
        i++;
    return i;
}
int EventReader::lastEarlierThan(int timestamp) const{
    uint i = 0;

    if(timestamp>p_end_timestamp)
        i = events.size()-1;
    else if(timestamp<p_start_timestamp)
        i = 0;
    else
        i = parts_index[(timestamp-p_start_timestamp)/PART_SIZE]-1;
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

Overlap EventReader::overlap(EventReader* other, EventReader* other2){
    Overlap rtn{0,{0,0,0},{0,0,0}};
    if(other2==nullptr){
        int from, to, i[] = {0,0};
        while(numberOfRuns()>i[0] && other->numberOfRuns()>i[1]){
            from = max(runStart(i[0]),other->runStart(i[1]));
            to = min(runEnd(i[0]),other->runEnd(i[1]));
            if(to-from>0){
                rtn.measureTime+=to-from;
                auto max = lastEarlierThan(to);
                for(int i=firstOlderThan(from);i<=max;i++){
                    if(item(i).isCalib())
                        rtn.calibration_events[0]++;
                    else
                        rtn.normal_events[0]++;
                }
                max = other->lastEarlierThan(to);
                for(int i=other->firstOlderThan(from);i<=max;i++){
                    if(other->item(i).isCalib())
                        rtn.calibration_events[1]++;
                    else
                        rtn.normal_events[1]++;
                }
            };
            if(other->runEnd(i[1])>runEnd(i[0]))
                i[0]++;
            else
                i[1]++;
        }
    }else{
        int from, to, i[] = {0,0,0};
        EventReader *ers[] = {this, other, other2};
        while(ers[0]->numberOfRuns()>i[0] && ers[1]->numberOfRuns()>i[1] && ers[2]->numberOfRuns()>i[2]){
            from = max(max(ers[0]->runStart(i[0]), ers[1]->runStart(i[1])), ers[2]->runStart(i[2]));
            to = min(min(ers[0]->runEnd(i[0]), ers[1]->runEnd(i[1])), ers[2]->runEnd(i[2]));
            if(to-from>0){
                rtn.measureTime+=to-from;
                int max;
                for(int k=0;k<3;k++){
                    max = ers[k]->lastEarlierThan(to);
                    for(int j=ers[k]->firstOlderThan(from);j<=max;j++){
                        if(ers[k]->item(j).isCalib())
                            rtn.calibration_events[k]++;
                        else
                            rtn.normal_events[k]++;
                    }
                }
            }
            if(ers[0]->runEnd(i[0])==to)
                i[0]++;
            else if(ers[1]->runEnd(i[1])==to)
                i[1]++;
            else
                i[2]++;
        }
    }
    return rtn;
}

uint32_t* EventReader::fileFromTo(char* filename){
    static uint32_t fromto[2];
    fromto[0] = 0;
    fromto[1] = 0;
    ifstream in(filename);
    char line[90];
    in.read(line,16);
    if(line[0]=='x' || line[0]=='c' || line[0]=='a'){
        in.seekg(0);
        in.getline(line,90);
        if(line[0]=='x')
            in.getline(line,90);
        int year,month,day,hour,minute,second;
        if(sscanf(line+2,"%d %d %d %d %d %d",&year,&month,&day,&hour,&minute,&second)!=6)
            return fromto;
        fromto[0] = date(year,month,day,hour,minute,second);
        in.seekg(-100,ios_base::end);
        in.getline(line,90);
        in.getline(line,90);
        if(sscanf(line+2,"%d %d %d %d %d %d",&year,&month,&day,&hour,&minute,&second)!=6){
            fromto[0] = 0;
            return fromto;
        };
        fromto[1] = date(year,month,day,hour,minute,second);
    }else{
        line[16]='\0';
        const string head = "CzeltaDataFile.1";
        if(head!=line){
            return fromto;
        }
        in.read(line,4);
        fromto[0] = *(uint32_t*)line;
        in.seekg(-32,ios_base::end);
        in.read(line,4);
        fromto[1] = *(uint32_t*)line;
    }
    return fromto;
}

void EventReader::setStation(uint8_t station){
    _station = station;
    for(int i=0;i<numberOfEvents();i++){
        item(i).setStation(station);
    }
}
