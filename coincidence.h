/*
 * Author: Martin Quarda
 */
#ifndef COINCIDENCE_H
#define COINCIDENCE_H
#include "event_reader.h"
#include "event.h"
#include "station.h"
class Coincidence
{
public:
#define LIMIT_N_COINCIDENCE 4
    Coincidence();
    void calc(double limit, bool save_events = true);
    int n;
    EventReader *readers[LIMIT_N_COINCIDENCE];
    uint8_t stations[LIMIT_N_COINCIDENCE];
    bool events_saved;
    vector<double> delta;
    vector<Event> events[LIMIT_N_COINCIDENCE];
    double limit;
    int numberOfCoincidences;
    Overlap overlap;
    double medium_value;
    double chance;
};

#endif // COINCIDENCE_H
