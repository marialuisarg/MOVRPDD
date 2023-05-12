#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Route.h"
#include "Node.h"
#include "Graph.h"

class Solution {
    private:
        int QT;
        vector<Route> routes;
        vector<pair<int,bool>> attendedClients;                      // identifies by ID
        vector<tuple<int, int, double, int, int>> candidatesCost;    // (nodeID, route, delta, prevNode, nextNode)

        pair<double,double> totalEnergyConsumption;                  // f1 (truck, truck+drone)
        pair<double,double> totalDeliveryCost;                       // f2 (truck, truck+drone)   
        pair<double,double> totalDeliveryTime;                       // f3 (truck, truck+drone)
        
        void sortCandidatesByCost(Graph *g);
        bool allClientsAttended(Graph *g);
        void insertRandomizedFirstClients(Graph *g);

    public:
        Solution(Graph *g, int QT);
        ~Solution();

        vector<Route> getRoutes();
        vector<pair<int,bool>> getAttendedClients();
        Node* getClosestClient(Graph *g, int from); 
        pair<double,double> getTotalDeliveryCost();
        pair<double,double> getTotalEnergyConsumption();
        pair<double,double> getTotalDeliveryTime();
        void getCheapestInsertion(Graph *g, vector<Node*> candidates, Node**client);

        void createTruckRoutes(Graph *g);
        void createDroneRoutes(Graph *g);
        
        void sortTruckClientsByCost(Graph *g, vector<pair<int,double>> clientsByCost, Route *r);
        void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode);
        void sortListByGain(vector<tuple<int, int, int, double, bool>> *list);

        void updateAttendedClients(int clientID);
        void updateCandidatesList(Node *client, Route *r, Graph *g, int iRoute);
        void updateSearchRange(vector<int> *searchRange, int rNode);
        void updateSolution(Graph *g, int typeOfSolution);

        bool includeClient(Node *client, Route *r, Graph *g, int prevNode, int iRoute);
        bool isReachableByDrone(Graph *g, tuple<int, int, int> flight, int routeIndex);

        void registerPrevCost();

        void printSolution();
        void printRoutes();
        void printCandidatesCost();

        void plotSolution(Solution *s, string instance);
        void plotTruckSolution(Solution *s, string instance);
};

#endif // SOLUTION_H_INCLUDED