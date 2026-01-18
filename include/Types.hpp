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

enum executionType {
    LIT,
    LIT_LS,
    ADPT,
    ADPT_LS,
    INVALID
};

// parameter settings of MOVRPDD Model //

const int   WT = 1500,      // tare weight of trucks
            WD = 25,        // tare weight of drones
            QD = 5,         // maximum load capacity of drones
            QT = 1000,      // maximum load capacity of trucks
            CT = 25,        // travel cost of trucks per unit distance
            CD = 1,         // travel cost of drones per unit distance
            CB = 500,       // basis cost of using truck equipped with drone
            ST = 60,        // average travel speed of trucks
            SD = 65;        // average travel speed of drones

const float E = 0.5;        // maximum endurance of empty drones

static const int INF = 99999;

struct SplitResult {
    std::vector<float> costs;
    std::vector<int> predecessors;
};

#endif // TYPES_H_