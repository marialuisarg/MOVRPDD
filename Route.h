#ifndef ROUTE_H_INCLUDED
#define ROUTE_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Node.h"

#define TRUCK_DRONE 1
#define TRUCK 2
#define DRONE 3

class Route {
    private:
        vector<Node*> truckRoute;
        // vector<tuple<int, int, int>> droneRoute;    // launch node, client, retrieval node 
        double cost;
        double currentTruckCapacity;
        double currentDroneCapacity;
        int numClients;
        
    public:
        Route(double truckCapacity, double droneCapacity, Node* depot);
        ~Route();
        void updateCost(int CT);
        void updateCapacity(double capacity);
        double getCost();
        double getCurrentCapacity();
        vector<Node*> getTruckRoute();
        vector<tuple<int, int, int>> getDroneRoute();
        int getNumClients();
        void updateNumClients();
        bool insertClient(Node *client, int prevNode);  // returns false if client can't be inserted
        Node* getNode(int position);
        Node* getPrevNode(int position);
        Node* getNextNode(int position);
        void printRoute();
};

#endif // ROUTE_H_INCLUDED