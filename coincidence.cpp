/*
 * Author: Martin Quarda
 */
#include "coincidence.h"

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
    this->limit = limit;
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
        double lambda[3];
        for(int k=0;k<3;k++)
            lambda[k] = double(overlap.measureTime)/double(overlap.normal_events[k]);
        medium_value = 3*(limit*limit)*overlap.measureTime/lambda[0]/lambda[1]/lambda[2];
    }
    for(int k = 0; k<(numberOfCoincidences==0?1:numberOfCoincidences); k++){
        double i = pow(medium_value,k)*exp(-medium_value)/fac(k);
        if(0.5*i==i || i!=i) break;
        chance -= i;
    }
    if(chance<0)chance=0;
}
