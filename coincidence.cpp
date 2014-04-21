/*
 * Author: Martin Quarda
 */
#include "coincidence.h"
#include "cstring"

Coincidence::Coincidence()
{
    n = 2;
}

double fac(int n)
{
    double rtn = 1;
    if(n<=1)return 1;
    for(int i = n;i>0;i--)
        rtn *= i;
    return rtn;
}

void Coincidence::calc(double limit, bool save_events){
    //reset all values
    events_saved = save_events;
    numberOfCoincidences = 0;
    int i[LIMIT_N_COINCIDENCE];
    if(n<2)n=2;
    if(n>LIMIT_N_COINCIDENCE)n=LIMIT_N_COINCIDENCE;
    for(int _i=0;_i<LIMIT_N_COINCIDENCE;_i++){
        i[_i] = 0;
        events[_i].clear();
    }
    events[0].clear();
    events[1].clear();
    events[2].clear();
    delta.clear();
    dirs.clear();
    this->limit = limit;
    
    
    //limit converted to tenth of nanosecond
    int64_t _limit = static_cast<int64_t>(limit*10000000000);
    if(n==2){
        overlap = readers[0]->overlap(readers[1]);
        std::function<void(int,int)> find_2coincidence = [this,_limit,&find_2coincidence](int i, int j){
            if(i >= readers[0]->numberOfEvents() || j >= readers[1]->numberOfEvents())
                return;
            int64_t a = readers[0]->item(i).tenthOfNSTimestamp()
                  , b = readers[1]->item(j).tenthOfNSTimestamp();
            if(abs(a-b)<_limit){
                if(!readers[0]->item(i).isCalib() && !readers[1]->item(j).isCalib()){
                    numberOfCoincidences++;
                    delta.push_back(abs(a-b)/1e10);
                    if(events_saved){
                        events[0].push_back(readers[0]->item(i));
                        events[1].push_back(readers[1]->item(j));
                    }
                }
                find_2coincidence(i+(a>=b?1:0),j+(a<b?1:0));
            }
        };
        while(i[0]<readers[0]->numberOfEvents() && i[1]<readers[1]->numberOfEvents()){
            find_2coincidence(i[0],i[1]);
            if(readers[0]->item(i[0]).tenthOfNSTimestamp() > readers[1]->item(i[1]).tenthOfNSTimestamp())
                i[1]++;
            else
                i[0]++;
        }
        medium_value = 0, chance = 1;
        double lambda[2];
        for(int k=0;k<2;k++)
            lambda[k] = double(overlap.measureTime)/double(overlap.normal_events[k]);
        medium_value = 2*overlap.measureTime*limit/lambda[0]/lambda[1];
    }else if(n==3){
        overlap = readers[0]->overlap(readers[1], readers[2]);
        //auto try to find coincidences and overlap coincidences, return index of reader which should be adjected
        std::function<int(int*)> find_3coincidence = [this, _limit, &find_3coincidence](int* i)->int{
            if(i[0] >= readers[0]->numberOfEvents() 
            || i[1] >= readers[1]->numberOfEvents()
            || i[2] >= readers[2]->numberOfEvents())
                return 0;
            int64_t t[3];
            t[0] = readers[0]->item(i[0]).tenthOfNSTimestamp();
            int64_t min = t[0]
                  , max = t[0];
            int min_index = 0
              , max_index = 0
              , med_index = 0;
            for(int k=1;k<3;k++){
                t[k] = readers[k]->item(i[k]).tenthOfNSTimestamp();
                if(t[k]>max){
                    max = t[k];
                    max_index = k;
                }else if(t[k]<min){
                    min = t[k];
                    min_index = k;
                }
            }
            med_index = min_index!=0&&max_index!=0?0:min_index!=1&&max_index!=1?1:2;
            if(max-min<_limit && !readers[min_index]->item(i[min_index]).isCalib()){
                if(!readers[med_index]->item(i[med_index]).isCalib() 
                && !readers[max_index]->item(i[max_index]).isCalib()){
                    numberOfCoincidences++;
                    delta.push_back((max-min)/1e10);
                    if(events_saved){
                        events[0].push_back(readers[0]->item(i[0]));
                        events[1].push_back(readers[1]->item(i[1]));
                        events[2].push_back(readers[2]->item(i[2]));
                    }
                    
                    //little chance of finding one triple coincidence multiple times
                    ++i[med_index];
                    find_3coincidence(i);
                    --i[med_index];
                    ++i[max_index];
                    find_3coincidence(i);
                    --i[max_index];
                }
            }
            return min_index;
        };
        while(i[0]<readers[0]->numberOfEvents()
           && i[1]<readers[1]->numberOfEvents()
           && i[2]<readers[2]->numberOfEvents()){
            i[find_3coincidence(i)]++;
        }
        
        //if we have all stations of triple coincidence - try to calculate direction of event based on GPS of stations
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
        
        //expected value of numberOfCoincidences -> random coincidences
        double lambda[3];
        for(int k=0;k<3;k++)
            lambda[k] = double(overlap.measureTime)/double(overlap.normal_events[k]);
        medium_value = 3*(limit*limit)*overlap.measureTime/lambda[0]/lambda[1]/lambda[2];
    }
    //chance of expected value -> better is just look to expected value
    for(int k = 0; k<(numberOfCoincidences==0?1:numberOfCoincidences); k++){
        double i = pow(medium_value,k)*exp(-medium_value)/fac(k);
        if(0.5*i==i || i!=i) break;
        chance -= i;
    }
    //this should never happen, but never say never
    if(chance<0)chance=0;
}
