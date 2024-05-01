#ifndef TYPES_H_
#define TYPES_H_

#include <iostream>
#include <vector>
#include <string>
#include <random>

#define GREEDY          0
#define RANDOM_GREEDY   1
#define RANDOM          2

enum nodeType {
    DEPOT = 0,
    TRUCK_DRONE = 1,
    TRUCK = 2,
    DRONE = 3
};

// parameter settings of MOVRPDD Model //

struct parameters {
    int WT = 1500;      // tare weight of trucks
    int WD = 25;        // tare weight of drones
    int QD = 5;         // maximum load capacity of drones
    int QT = 1000;      // maximum load capacity of trucks
    int CT = 25;        // travel cost of trucks per unit distance
    int CD = 1;         // travel cost of drones per unit distance
    int CB = 500;       // basis cost of using truck equipped with drone
    float E = 0.5;      // maximum endurance of empty drones
    int ST = 60;        // average travel speed of trucks
    int SD = 65;        // average travel speed of drones
};

static const unsigned int INF = 99999;

#endif // TYPES_H_