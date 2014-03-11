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

void Coincidence::calc(double limit){
    numberOfCoincidences = 0;
    events[0].clear();
    events[1].clear();
    delta.clear();
    this->limit = limit;
    int _limit = static_cast<int>(limit*10000000000);
    overlap = readers[0]->overlap(*readers[1]);
    int i[] = {0,0};
    int64_t a,b;
    while(i[0]<readers[0]->numberOfEvents() && i[1]<readers[1]->numberOfEvents()){
        if(!readers[0]->_item(i[0]).isCalib() && !readers[1]->_item(i[1]).isCalib()){
            a = readers[0]->_item(i[0]).tenthOfNSTimestamp();
            b = readers[1]->_item(i[1]).tenthOfNSTimestamp();
            if(abs(a-b)<_limit){
                numberOfCoincidences++;
                delta.push_back(abs(a-b)/1e10);
                events[0].push_back(readers[0]->_item(i[0]));
                events[1].push_back(readers[1]->_item(i[1]));
            }
        }
        if(a>b)
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
    chance = abs(chance);
}
