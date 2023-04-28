#ifndef ROUTE_H_INCLUDED
#define ROUTE_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Node.h"
#include "Graph.h"

#define TRUCK_DRONE 1
#define TRUCK 2
#define DRONE 3

class Route {
    private:
        vector<Node*> truckRoute;
        vector<tuple<int, int, int>> droneRoute;    // launch node, client, retrieval node 
        vector<int> prevTruckRoute;                 // final truck route before drone flights
        
        double cost;
        double prevCost;
        double currentTruckCapacity;
        double currentDroneCapacity;
        int numClients;
        
    public:
        Route(double truckCapacity, double droneCapacity, Node* depot);
        ~Route();
        void updateCost(Graph *g, int CT, int CD, int CB);
        void updateCapacity(double capacity);
        double getCost();
        double getPrevCost();
        double getCurrentCapacity();
        vector<Node*> getTruckRoute();
        vector<int> getPrevTruckRoute();
        vector<tuple<int, int, int>> getDroneRoute();
        int getNumClients();
        void updateNumClients();
        bool insertClient(Node *client, int prevNode);  // returns false if client can't be inserted
        void insertDroneFlight(tuple<int,int,int> flight);
        Node* getNode(int position);
        Node* getPrevNode(int position);
        Node* getNextNode(int position);
        void printRoute();
        void removeClientsServedByDrone(Graph *g, int CT, int CD, int CB);
        void registerPrevTruckRoute();
};

#endif // ROUTE_H_INCLUDED