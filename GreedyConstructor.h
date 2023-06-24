#ifndef GREEDYCONSTRUCTOR_H_INCLUDED
#define GREEDYCONSTRUCTOR_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Route.h"
#include "Node.h"
#include "Graph.h"
#include "Solution.h"

namespace ns_GreedyConstructor {
        vector<pair<int,bool>> attendedClients;                      // identifies by ID
        vector<tuple<int, int, double, int, int>> candidatesCost;    // (nodeID, route, delta, prevNode, nextNode)

        int numRoutes;
        bool droneRouteCreated;

        Solution * greedyConstructor(Graph *g, int QT);
        
        void sortCandidatesByCost(Graph *g, Solution *sol);
        bool allClientsAttended(Graph *g, Solution *sol);
        void insertRandomizedFirstClients(Graph *g, Solution *sol);

        int getNumRoutes(Solution *sol) { return sol->getNumRoutes(); };
        vector<pair<int,bool>> getAttendedClients() { return attendedClients; };
        bool verifyNeighbor(Route *r, int currentPrevIndex, int currentNextIndex);
        bool isInSearchRange(vector<int> searchRange, int clientID);

        void createTruckRoutes(Graph *g, Solution *sol);
        void createDroneRoutes(Graph *g, Solution *sol);
        
        void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode);
        void sortListByGain(vector<tuple<int, int, int, double, bool>> *list);

        void updateAttendedClients(int clientID);
        void updateCandidatesList(Node *client, Solution *sol, Route *r, Graph *g, int iRoute);
        void updateSearchRange(vector<int> *searchRange, int rNode);

        bool includeClient(Node *client, Solution *sol, Route *r, Graph *g, int prevNode, int iRoute);
        bool isReachableByDrone(Graph *g, Solution *sol, tuple<int, int, int> flight, int routeIndex);

        void plotSolution(string instance, int i, Solution *sol) { sol->plotSolution(instance, i); };
        void printCandidatesCost(Solution *sol);
        void freeMemory();
}

#endif // GREEDYCONSTRUCTOR_H_INCLUDED