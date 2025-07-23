#ifndef SOLUTION_H_INCLUDED
#define SOLUTION_H_INCLUDED

using namespace std;

#include <string>
#include <vector>
#include <tuple>
#include <deque>

#include "Route.hpp"
#include "Node.hpp"
#include "Graph.hpp"
#include "RandomGenerator.hpp"

class Solution {
    private:
        
        int QT;                                                     // maximum truck capacity      
        int numRoutes;                                              // number of routes
        int numClients;                                             // number of clients
        vector<Route> routes;                                       // list of routes
        vector<pair<int, bool>> attendedClients;                    // (clientID, attended)
        vector<tuple<int, int, double, int, int>> candidatesCost;   // (clientID, routeIndex, cost, prevNode, nextNode)
        bool drone;

        vector<int> giantTour;                                      // giant tour of all clients (used for literature constructor)
        vector<int> predecessors;                                   // used for decomposing giant tour into routes
        
        int         rank;                                           // ranking for fast non-dominated sort
        int         dominatedBy;                                    // number of solutions that dominate this solution
        double      crDistance;                                     // crowding distance
        vector<int> dominatedSolutions;                             // list of dominated solutions indexes

        double totalEnergyConsumption;                  // f1 
        double totalDeliveryCost;                       // f2    
        double totalDeliveryTime;                       // f3 

        RandomGenerator* randGen;

    public:
        Solution(Graph *g, int QT, RandomGenerator *randGen);
        ~Solution();

        void setNumClients(int numClients) { this->numClients = numClients-1; };
        void setAttendedClient(int i, bool value) { this->attendedClients[i].second = value; };
        void setAttendedClients(vector<pair<int, bool>> attendedClients) { this->attendedClients = attendedClients; }; 
        void setRank(int rank) { this->rank = rank; };
        void setCrDistance(double dist) { this->crDistance = dist; };
        void setDominatedSolutions(vector<int> dominatedSolutions) { this->dominatedSolutions = dominatedSolutions; };
        void setDominatedBy(int dominatedBy) { this->dominatedBy = dominatedBy; };
        void setCandidateCost(int i, tuple<int, int, double, int, int> value) { this->candidatesCost[i] = value; };
        void setCandidatesCost(vector<tuple<int, int, double, int, int>> candidatesCost) { this->candidatesCost = candidatesCost; };
        void setPredecessors(vector<int> predecessors) { this->predecessors = predecessors; };
        void setGiantTour(std::vector<int> giantTour) { this->giantTour = giantTour; };
        
        int                                         getRank() { return this->rank; };
        vector<pair<int, bool>>                     getAttendedClients() { return this->attendedClients; }
        pair<int, bool>                             getAttendedClient(int i) { return this->attendedClients[i]; };
        vector<int>                                 getDominatedSolutions() { return this->dominatedSolutions; };
        int                                         getDominatedBy() { return this->dominatedBy; };
        vector<tuple<int, int, double, int, int>>&  getCandidatesCost() { return this->candidatesCost; };
        tuple<int, int, double, int, int>           getCandidateCost(int i) { return this->candidatesCost[i]; };
        double                                      getObjective(int i);
        double                                      getCrDistance() { return this->crDistance; };
        vector<Route>                               getRoutes();
        Route*                                      getRoute(int i) { return &(this->routes[i]); };
        int                                         getQT() { return this->QT; };
        int                                         getNumRoutes() { return this->numRoutes; };
        int                                         getNumClients() { return this->numClients; };
        std::vector<int>                            getGiantTour() { return this->giantTour; };
        vector<int>                                 getPredecessors() { return this->predecessors; };
        bool                                        droneIsUsed() { return this->drone; };

        void eraseCandidateCostAt(int i);
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

        void includeRoute(Route* route);

        void updateSolution(Graph *g);
        bool allClientsAttended(Graph *g);

        void printSolution();
        void printRoutes();
        void printEncodedSolution(vector<int> sol);
        void saveRouteToPlot(ofstream &output_file);

        void plotSolution(string instance, int i, string fileName);

        vector<int> encode();
};

#endif // SOLUTION_H_INCLUDED