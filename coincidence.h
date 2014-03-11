#ifndef COINCIDENCE_H
#define COINCIDENCE_H
#include "event_reader.h"
#include "event.h"
class Coincidence
{
public:
    Coincidence();
    void calc(double limit);
    EventReader *readers[2];
    vector<double> delta;
    vector<_Event> events[2];
    double limit;
    int numberOfCoincidences;
    Overlap overlap;
    double medium_value;
    double chance;
};

#endif // COINCIDENCE_H
