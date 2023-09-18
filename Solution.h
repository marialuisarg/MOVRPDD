#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED

using namespace std;

#include <string>
#include <vector>
#include <tuple>

#include "Route.h"
#include "Node.h"
#include "Graph.h"

class Solution {
    private:
        int QT;                                                     // maximum truck capacity      
        int numRoutes;                                              // number of routes
        int numClients;                                             // number of clients
        vector<Route> routes;                                       // list of routes
        vector<pair<int, bool>> attendedClients;                    // (clientID, attended)
        vector<tuple<int, int, double, int, int>> candidatesCost;   // (clientID, routeIndex, cost, prevNode, nextNode)
        bool drone;

        double totalEnergyConsumption;                  // f1 
        double totalDeliveryCost;                       // f2    
        double totalDeliveryTime;                       // f3 

    public:
        Solution(Graph *g, int QT);
        ~Solution();

        void setNumClients(int numClients) { this->numClients = numClients-1; };
        vector<pair<int, bool>> getAttendedClients() { return this->attendedClients; }
        pair<int, bool> getAttendedClient(int i) { return this->attendedClients[i]; };
        void setAttendedClient(int i, bool value) { this->attendedClients[i].second = value; };
        void setAttendedClients(vector<pair<int, bool>> attendedClients) { this->attendedClients = attendedClients; };
        
        vector<tuple<int, int, double, int, int>> getCandidatesCost() { return this->candidatesCost; };
        tuple<int, int, double, int, int> getCandidateCost(int i) { return this->candidatesCost[i]; };
        void setCandidateCost(int i, tuple<int, int, double, int, int> value) { this->candidatesCost[i] = value; };
        void setCandidatesCost(vector<tuple<int, int, double, int, int>> candidatesCost) { this->candidatesCost = candidatesCost; };
        void eraseCandidateCostAt(int i);

        vector<Route> getRoutes();
        Route* getRoute(int i) { return &(this->routes[i]); };
        int getQT() { return this->QT; };
        int getNumRoutes() { return this->numRoutes; };
        int getNumClients() { return this->numClients; };
        bool droneIsUsed() { return this->drone; };

        bool includeClient(Node* client, Graph *g, int prevNode, int routeIndex);
        void sortCandidatesByCost(Graph *g);
        void updateAttendedClients(int clientID);
        void updateCandidatesList(Node *client, Graph *g, int iRoute);

        void setTotalEnergyConsumption(double totalEnergyConsumption) { this->totalEnergyConsumption = totalEnergyConsumption; };
        void setTotalDeliveryCost(double totalDeliveryCost) { this->totalDeliveryCost = totalDeliveryCost; };
        void setTotalDeliveryTime(double totalDeliveryTime) { this->totalDeliveryTime = totalDeliveryTime; };

        double getTotalDeliveryCost() { return this->totalDeliveryCost; };
        double getTotalEnergyConsumption() { return this->totalEnergyConsumption; };
        double getTotalDeliveryTime() { return this->totalDeliveryTime; };

        void setNumRoutes(int numRoutes) { this->numRoutes = numRoutes; };
        void setDroneRouteCreated(bool droneRouteCreated) { this->drone = droneRouteCreated; };
        void createRoute(Route r) { this->routes.push_back(r); };

        bool dominates(Solution *s);
        unsigned int random(int min, int max);

        void updateSolution(Graph *g);
        bool allClientsAttended(Graph *g);

        void printSolution();
        void printRoutes();
        void printEncodedSolution(vector<int> sol);

        void plotSolution(string instance, int i);

        vector<int> encode();
};

#endif // SOLUTION_H_INCLUDED