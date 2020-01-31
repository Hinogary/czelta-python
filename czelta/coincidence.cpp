/*
 * Author: Martin Quarda
 */
#include "coincidence.h"
#include <cstring>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>

std::ostream&
operator<<( std::ostream& dest, __int128_t value )
{
    std::ostream::sentry s( dest );
    if ( s ) {
        __uint128_t tmp = value < 0 ? -value : value;
        char buffer[ 128 ];
        char* d = std::end( buffer );
        do
        {
            -- d;
            *d = "0123456789"[ tmp % 10 ];
            tmp /= 10;
        } while ( tmp != 0 );
        if ( value < 0 ) {
            -- d;
            *d = '-';
        }
        int len = std::end( buffer ) - d;
        if ( dest.rdbuf()->sputn( d, len ) != len ) {
            dest.setstate( std::ios_base::badbit );
        }
    }
    return dest;
}

Coincidence::Coincidence()
{
    n = 0;
}

void Coincidence::resize(int n){
    this->n = n;
    readers.resize(n);
    stations.resize(n);
    events.resize(n);
}

double fac(int n)
{
    double rtn = 1;
    if(n<=1)return 1;
    for(int i = n;i>0;i--)
        rtn *= i;
    return rtn;
}

struct heap_struct{
    __int128 time;
    int index;
    int k; // EventReader index
};


struct heap_comparator{
    bool operator()(const heap_struct& left, const heap_struct& right){
        return left.time > right.time;
    }
};

void Coincidence::calc(double limit, bool save_events){
    //reset all values
    events_saved = save_events;
    numberOfCoincidences = 0;
    vector<int> i;

    i.resize(n);
    for(int _i=0;_i<n;_i++){
        i[_i] = 0;
        events[_i].clear();
    }
    delta.clear();
    dirs.clear();
    this->limit = limit;
    //limit converted to tenth of nanosecond
    uint64_t _limit = static_cast<uint64_t>(limit*1e10);

    vector<heap_struct> structure;
    structure.resize(n);
    __int128 c_max = 0;
    for(int i=0; i<n; i++){
        structure[i].time = readers[i]->item(0).tenthOfNSTimestamp();
        c_max = max(structure[i].time, c_max);
        structure[i].index = 0;
        structure[i].k = i;
        cout<<structure[i].time<<","<<c_max<<","<<structure[i].index<<","<<structure[i].k<<endl;
    }
    make_heap(structure.begin(), structure.end(), heap_comparator());

    int m=0;

    std::function<void(__int128,__int128,int,vector<heap_struct>)> rec_find_coincidence =
    [&](__int128 min, __int128 maxmax, int k, vector<heap_struct> structure){
        for(;;){
            pop_heap(structure.begin(), structure.end(), heap_comparator());
            auto current = structure.back();
            structure.pop_back();

            i[current.k] = current.index;

            if(k>1){
                rec_find_coincidence(min, maxmax, k-1, structure);
            }else{
                for(int _i=0; _i<n; _i++){
                    events[_i].push_back(readers[_i]->item(i[_i]));
                }
                numberOfCoincidences++;
                delta.push_back((current.time-min)/1e10);
            }

            current.index += 1;
            auto& reader = *readers[current.k];
            if(current.index >= reader.numberOfEvents()) return;
            current.time = reader.item(current.index).tenthOfNSTimestamp();
            if(current.time > maxmax) return;
            structure.push_back(current);
            push_heap(structure.begin(), structure.end(), heap_comparator());
        }
    };

    // LAUNCH main logic here!
    for(;;){
        pop_heap(structure.begin(), structure.end(), heap_comparator());
        auto current = structure.back();
        structure.pop_back();
        if(c_max - current.time <= _limit){
            i[current.k] = current.index;
            auto maxmax = current.time + _limit;
            rec_find_coincidence(current.time, maxmax, n-1, structure);
        }
        current.index += 1;
        auto& reader = *readers[current.k];
        if(current.index >= reader.numberOfEvents()) return;
        current.time = reader.item(current.index).tenthOfNSTimestamp();
        c_max = max(c_max, current.time);
        structure.push_back(current);
        push_heap(structure.begin(), structure.end(), heap_comparator());
    }


    // calculating statistics
    // TODO: find equation to k detectors
    if(n == 2){
        medium_value = 0, chance = 1;
        double lambda[2];
        for(int k=0;k<2;k++)
            lambda[k] = double(overlap.measureTime)/double(overlap.normal_events[k]);
        medium_value = 2*overlap.measureTime*limit/lambda[0]/lambda[1];
    }
    if(n == 3){
        calc_direction_triple();
        // expected value of numberOfCoincidences -> random coincidences
        double lambda[3];
        for(int k=0;k<3;k++)
            lambda[k] = double(overlap.measureTime)/double(overlap.normal_events[k]);
        medium_value = 3*(limit*limit)*overlap.measureTime/lambda[0]/lambda[1]/lambda[2];
    }
    if(n == 2 or n == 3){
        // chance of expected value -> better is just look to expected value
        for(int k = 0; k<(numberOfCoincidences==0?1:numberOfCoincidences); k++){
            double i = pow(medium_value,k)*exp(-medium_value)/fac(k);
            if(0.5*i==i || i!=i) break;
            chance -= i;
        }
    }

    //this should never happen, but never say never
    if(chance<0)chance=0;
}


void Coincidence::calc_direction_triple(){
    if(stations[0]!=0 && stations[1]!=0 && stations[2]!=0 && events_saved){
        double stPos[4];
#define x1 stPos[0]
#define y1 stPos[1]
#define x2 stPos[2]
#define y2 stPos[3]
        double f_gps_pos[2];
        double s_gps_pos[2];
        //avg gps pos -> middle of stations
        double a_gps_pos[2];
        memcpy(f_gps_pos, Station::getStation(stations[0]).GPSPosition(), sizeof(double)*2);
        memcpy(s_gps_pos, Station::getStation(stations[1]).GPSPosition(), sizeof(double)*2);

        a_gps_pos[0]+=f_gps_pos[0];a_gps_pos[1]+=f_gps_pos[1];
        a_gps_pos[0]+=s_gps_pos[0];a_gps_pos[1]+=s_gps_pos[1];

        //calculating of x1
        s_gps_pos[0] = f_gps_pos[0];
        x1 = deltaDistance(f_gps_pos, s_gps_pos)*1000;
        if(f_gps_pos[1] > s_gps_pos[1])x1 = -x1;

        //calculating of y1
        memcpy(s_gps_pos, Station::getStation(stations[1]).GPSPosition(), sizeof(double)*2);
        s_gps_pos[1] = f_gps_pos[1];
        y1 = deltaDistance(f_gps_pos, s_gps_pos)*1000;
        if(f_gps_pos[0] > s_gps_pos[0])y1 = -y1;

        //calculating of x2
        memcpy(s_gps_pos, Station::getStation(stations[2]).GPSPosition(), sizeof(double)*2);
        a_gps_pos[0]+=s_gps_pos[0];a_gps_pos[1]+=s_gps_pos[1];
        s_gps_pos[0] = f_gps_pos[0];
        x2 = deltaDistance(f_gps_pos, s_gps_pos)*1000;
        if(f_gps_pos[1] > s_gps_pos[1])x2 = -x2;

        //calculating of y2
        memcpy(s_gps_pos, Station::getStation(stations[2]).GPSPosition(), sizeof(double)*2);
        s_gps_pos[1] = f_gps_pos[1];
        y2 = deltaDistance(f_gps_pos, s_gps_pos)*1000;
        if(f_gps_pos[0] > s_gps_pos[0])y2 = -y2;

        //avg gps pos
        a_gps_pos[0]/=3;a_gps_pos[1]/=3;

        //dirs
        for(int i=0;i<numberOfCoincidences;i++){
            double dir_vector[3];
            const double t1 = (events[1][i].timestamp()-events[0][i].timestamp())
                +(events[1][i].time_since_second()-events[0][i].time_since_second());
            const double t2 = (events[2][i].timestamp()-events[0][i].timestamp())
                +(events[2][i].time_since_second()-events[0][i].time_since_second());
#define vec_x dir_vector[0]
#define vec_y dir_vector[1]
#define vec_z dir_vector[2]
#define c2 (SPEED_OF_LIGHT*SPEED_OF_LIGHT)
                vec_x = c2*(t2*y1 - t1*y2)/
                    (x1*y2 - x2*y1);
                vec_y = c2*(t2*x1 - t1*x2)/
                    (y1*x2 - y2*x1);
                //vec_z squared
                vec_z = c2 - vec_x*vec_x - vec_y*vec_y;
                if (vec_z < 0){
                for(int i=0;i<4;i++)
                    dirs.push_back(0);
                continue;
                }
                vec_z = sqrt(vec_z);
                float* AH = dirVectorToAh(dir_vector);
                if (AH==nullptr){
                for(int i=0;i<4;i++)
                    dirs.push_back(0);
                continue;
                }
                dirs.push_back(AH[0]/M_PI*180);
                dirs.push_back(AH[1]/M_PI*180);
                float* DRA = localToGlobalDirection(AH ,a_gps_pos, events[0][i].timestamp());
                dirs.push_back(DRA[0]/M_PI*180);
                dirs.push_back(DRA[1]/M_PI*180);
        }
    }
}
