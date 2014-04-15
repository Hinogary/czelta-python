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
    Coincidence();
    void calc(double limit, bool save_events = true);
    EventReader *readers[2];
    uint8_t stations[2];
    bool events_saved;
    vector<double> delta;
    vector<Event> events[2];
    double limit;
    int numberOfCoincidences;
    Overlap overlap;
    double medium_value;
    double chance;
};

#endif // COINCIDENCE_H
