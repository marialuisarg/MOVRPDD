#ifndef CONSTRUCTOR_HPP_
#define CONSTRUCTOR_HPP_

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <deque>
#include <unordered_map>

#include "Graph.hpp"
#include "Solution.hpp"
#include "Types.hpp"
#include "RandomGenerator.hpp"

namespace Constructor {
    void insertRandomizedFirstClients(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, RandomGenerator *rng);
    void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode);
    void sortListByGain(vector<tuple<int, int, int, double, bool>> *list);
    bool isReachableByDrone(Graph *g, Solution *sol, tuple<int, int, int> flight, int routeIndex);
    void updateSearchRange(vector<int>*searchRange, int rNode);
    void printCandidatesCost(Solution *sol);
    void createDroneRoutes(Graph *g, Solution *sol);
}

namespace GreedyConstructor {
    void createTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, RandomGenerator *rng);
    Solution* run(Graph *g, int QT);
}

namespace RandomConstructor {   
    void createRandomTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, double alpha, RandomGenerator *rng);
    vector<Solution*> run(Graph *g, int QT, double alpha, int numIterations, int setSize, RandomGenerator *rng);
}

namespace AdaptiveConstructor {
    void createAdaptiveTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated);
    vector<Solution*> run(Graph *g, int QT, int numIterations, int setSize, RandomGenerator *rng);
}

namespace LiteratureConstructor {
    vector<Solution*> run(Graph *g, int QT, RandomGenerator *randGen, int setSize);
    Solution* truckRouteSplit(std::deque<Node*> clients, Graph* g);
    void split(Graph* g, Solution* sol);
    deque<Route*> extract(const std::deque<Node*>& clients, const std::vector<int>& predecessorIndex, Graph* g);
}

#endif // CONSTRUCTOR_HPP_