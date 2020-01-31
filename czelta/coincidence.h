/*
 * Author: Martin Quarda
 */
#ifndef COINCIDENCE_H
#define COINCIDENCE_H
#include "event_reader.h"
#include "event.h"
#include "station.h"
#include "common_func.h"
#include <vector>

using namespace std;
class Coincidence
{
public:
    Coincidence();
    void calc(double limit, bool save_events = true);
    void calc_direction_triple();
    void resize(int n);
    int n;
    vector<EventReader*> readers;
    vector<uint8_t> stations;
    bool events_saved;
    vector<double> delta;
    vector<vector<Event>> events;
    vector<float> dirs;
    double limit;
    int numberOfCoincidences;
    Overlap overlap;
    double medium_value;
    double chance;
};

#endif // COINCIDENCE_H
