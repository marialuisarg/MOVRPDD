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
        vector<Route> routes;
        vector<pair<int,bool>> attendedClients;                      // identifies by ID
        vector<tuple<int, int, double, int, int>> candidatesCost;    // (nodeID, route, delta, prevNode, nextNode)
        double cost;
        
        void sortCandidatesByCost(Graph *g);
        bool allClientsAttended(Graph *g);
        void insertRandomizedFirstClients(Graph *g);

    public:
        Solution(Graph *g);
        ~Solution();

        vector<Route> getRoutes();
        vector<pair<int,bool>> getAttendedClients();
        void getCheapestInsertion(Graph *g, vector<Node*> candidates, Node**client);

        void createTruckRoutes(Graph *g);
        void createDroneRoutes(Graph *g);
        void updateAttendedClients(int clientID);
        void updateCandidatesList(Node *client, Route *r, Graph *g, int iRoute);
        bool includeClient(Node *client, Route *r, Graph *g, int prevNode, int iRoute);
        
        Node* getClosestClient(Graph *g, int from); 
        double getCost();
        void updateSolutionCost();

        void printSolution();
        void printRoutes();
        void printCandidatesCost();
        void plotSolution(Solution *s, string instance);
};

#endif // SOLUTION_H_INCLUDED