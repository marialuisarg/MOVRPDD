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
        int QT;                                         //  maximum truck capacity      
        int numRoutes;
        vector<Route> routes;
        vector<pair<int, bool>> * attendedClients;                    //  list of attended clients
        vector<tuple<int, int, double, int, int>> * candidatesCost;   // list of candidates costs
        bool drone;

        double totalEnergyConsumption;                  // f1 
        double totalDeliveryCost;                       // f2    
        double totalDeliveryTime;                       // f3 

    public:
        Solution(Graph *g, int QT);
        ~Solution();

        vector<pair<int, bool>> * getAttendedClients() { return this->attendedClients; }
        pair<int, bool> * getAttendedClient(int i) { return this->attendedClients[i]; };
        void setAttendedClient(int i, bool value) { this->attendedClients[i].second = value; };
        void setAttendedClients(vector<pair<int, bool>> attendedClients) { this->attendedClients = attendedClients; };
        
        vector<tuple<int, int, double, int, int>> getCandidatesCost() { return this->candidatesCost; };
        tuple<int, int, double, int, int> getCandidateCost(int i) { return this->candidatesCost[i]; };
        void setCandidateCost(int i, tuple<int, int, double, int, int> value) { this->candidatesCost[i] = value; };
        void setCandidatesCost(vector<tuple<int, int, double, int, int>> candidatesCost) { this->candidatesCost = candidatesCost; };

        vector<Route> getRoutes();
        Route* getRoute(int i) { return &(this->routes[i]); };
        int getQT() { return this->QT; };
        int getNumRoutes() { return this->numRoutes; };
        bool droneIsUsed() { return this->drone; };

        double getTotalDeliveryCost() { return this->totalDeliveryCost; };
        double getTotalEnergyConsumption() { return this->totalDeliveryTime; };
        double getTotalDeliveryTime() { return this->totalDeliveryTime; };

        void setNumRoutes(int numRoutes) { this->numRoutes = numRoutes; };
        void setDroneRouteCreated(bool droneRouteCreated) { this->drone = droneRouteCreated; };
        void createRoute(Route r) { this->routes.push_back(r); };

        bool dominates(Solution *s);

        void updateSolution(Graph *g);

        void printSolution();
        void printRoutes();

        void plotSolution(string instance, int i);
};

#endif // SOLUTION_H_INCLUDED