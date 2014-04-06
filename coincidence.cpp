/*
 * Author: Martin Quarda
 */
#include "coincidence.h"

Coincidence::Coincidence()
{

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
    events[0].clear();
    events[1].clear();
    delta.clear();
    this->limit = limit;
    int _limit = static_cast<int>(limit*10000000000);
    overlap = readers[0]->overlap(*readers[1]);
    int i[] = {0,0};
    std::function<void(int,int)> find_coincidence = [this,_limit,&find_coincidence](int i, int j){
        if(i >= readers[0]->numberOfEvents() || j >= readers[1]->numberOfEvents())
            return;
        int64_t a = readers[0]->item(i).tenthOfNSTimestamp()
              , b = readers[1]->item(j).tenthOfNSTimestamp();
        if(abs(a-b)<_limit){
            if(readers[0]->item(i).isCalib() || readers[1]->item(j).isCalib()){
                find_coincidence(i+(a>b?1:0),j+(a<b?1:0));
            }else{
                numberOfCoincidences++;
                delta.push_back(abs(a-b)/1e10);
                if(events_saved){
                    events[0].push_back(readers[0]->item(i));
                    events[1].push_back(readers[1]->item(j));
                }
                find_coincidence(i+(a>b?1:0),j+(a<b?1:0));
            }
        }
    };
    while(i[0]<readers[0]->numberOfEvents() && i[1]<readers[1]->numberOfEvents()){
        find_coincidence(i[0],i[1]);
        if(readers[0]->item(i[0]).tenthOfNSTimestamp() > readers[1]->item(i[1]).tenthOfNSTimestamp())
            i[1]++;
        else
            i[0]++;
    }
    medium_value = 0, chance = 1;
    double lambda[2];
    for(int k=0;k<2;k++)
        lambda[k] = double(overlap.measureTime)/double(overlap.normal_events[k]);
    //return array<double,2>{lambda[0],lambda[1]};
    medium_value = 2*overlap.measureTime*limit/lambda[0]/lambda[1];
    for(int k = 0; k<(numberOfCoincidences==0?1:numberOfCoincidences); k++){
        double i = pow(medium_value,k)*exp(-medium_value)/fac(k);
        if(0.5*i==i || i!=i) break;
        chance -= i;
    }
    if(chance<0)chance=0;
}
