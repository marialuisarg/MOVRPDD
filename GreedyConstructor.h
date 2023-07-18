#ifndef GREEDYCONSTRUCTOR_H_INCLUDED
#define GREEDYCONSTRUCTOR_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Route.h"
#include "Node.h"
#include "Graph.h"
#include "Solution.h"

// greedy constructor 
    Solution * greedyConstructor(Graph *g, int QT);
    void createTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated);
    void sortCandidatesByCost(Graph *g, Solution *sol);
    void insertRandomizedFirstClients(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated);
    bool allClientsAttended(Graph *g, Solution *sol);
    bool includeClient(Node *client, Solution *sol, Route *r, Graph *g, int prevNodeIndex, int iRoute);
    void updateAttendedClients(int clientID);
    bool verifyNeighbor(Route *r, int currentPrevIndex, int currentNextIndex);
    void updateCandidatesList(Node *client, Solution *sol, Route *r, Graph *g, int iRoute);
    bool isInSearchRange(vector<int> searchRange, int clientID);
    void createDroneRoutes(Graph *g, Solution *sol);
    void sortListByGain(vector<tuple<int, int, int, double, bool>> *list);
    void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode);
    bool isReachableByDrone(Graph *g, Solution *sol, tuple<int, int, int> flight, int routeIndex);
    void updateSearchRange(vector<int>*searchRange, int rNode);
    void printCandidatesCost(Solution *sol);

#endif // GREEDYCONSTRUCTOR_H_INCLUDED