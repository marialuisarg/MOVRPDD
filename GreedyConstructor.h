#ifndef GREEDYCONSTRUCTOR_H_INCLUDED
#define GREEDYCONSTRUCTOR_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Solution.h"
#include "Route.h"
#include "Node.h"
#include "Graph.h"

class GreedyConstructor {
    friend class Solution;
    private:
        Solution * solution;
        vector<pair<int,bool>> attendedClients;                      // identifies by ID
        vector<tuple<int, int, double, int, int>> candidatesCost;    // (nodeID, route, delta, prevNode, nextNode)

        int numRoutes;
        bool droneRouteCreated;
        
        void sortCandidatesByCost(Graph *g);
        bool allClientsAttended(Graph *g);
        void insertRandomizedFirstClients(Graph *g);

    public:
        GreedyConstructor(Graph *g, int QT);
        ~GreedyConstructor();

        int getNumRoutes() { return this->solution->getNumRoutes(); };
        vector<pair<int,bool>> getAttendedClients() { return this->attendedClients; };
        Node* getClosestClient(Graph *g, int from); 
        void getCheapestInsertion(Graph *g, vector<Node*> candidates, Node**client);
        bool verifyNeighbor(Route *r, int currentPrevIndex, int currentNextIndex);
        bool isInSearchRange(vector<int> searchRange, int clientID);

        void createTruckRoutes(Graph *g);
        void createDroneRoutes(Graph *g);
        
        void sortTruckClientsByCost(Graph *g, vector<pair<int,double>> clientsByCost, Route *r);
        void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode);
        void sortListByGain(vector<tuple<int, int, int, double, bool>> *list);

        void updateAttendedClients(int clientID);
        void updateCandidatesList(Node *client, Route *r, Graph *g, int iRoute);
        void updateSearchRange(vector<int> *searchRange, int rNode);
        void updateSolution(Graph *g);

        bool includeClient(Node *client, Route *r, Graph *g, int prevNode, int iRoute);
        bool isReachableByDrone(Graph *g, tuple<int, int, int> flight, int routeIndex);

        void plotSolution(string instance, int i) { this->solution->plotSolution(instance, i); };
        void printCandidatesCost();
};

#endif // GREEDYCONSTRUCTOR_H_INCLUDED