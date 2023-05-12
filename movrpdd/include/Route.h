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
        vector<int> prevTruckRoute;                 // final truck route before drone flights
        vector<tuple<int, int, int>> droneRoute;    // launch node, client, retrieval node 
    
        pair<double,double> energyConsumption;      // f1 (truck, truck+drone)
        pair<double,double> deliveryCost;           // f2 (truck, truck+drone
        pair<double,double> deliveryTime;           // f3 (truck, truck+drone)

        double currentTruckCapacity;
        double currentDroneCapacity;
        int numClients;
        
    public:
        Route(double truckCapacity, double droneCapacity, Node* depot);
        ~Route();

        void updateDeliveryCost(Graph *g, int CT, int CD, int CB, int typeOfRoute);
        void updateEnergyConsumption(Graph *g, int typeOfRoute, int QT);
        void updateDeliveryTime(Graph *g, int typeOfRoute, int VT, int VD);
        void updateCapacity(double capacity);
        void updateNumClients();

        void removeClientsServedByDrone(Graph *g, int CT, int CD, int CB);
        void registerPrevTruckRoute();

        double getDeliveryCost(int typeOfRoute);
        double getEnergyConsumption(int typeOfRoute);
        double getDeliveryTime(int typeOfRoute);
        double getPrevCost();
        double getCurrentCapacity();
        vector<Node*> getTruckRoute();
        vector<int> getPrevTruckRoute();
        vector<tuple<int, int, int>> getDroneRoute();
        int getNumClients();
        Node* getNode(int position);
        Node* getPrevNode(int position);
        Node* getNextNode(int position);

        bool insertClient(Node *client, int prevNode);  // returns false if client can't be inserted
        void insertDroneFlight(tuple<int,int,int> flight);

        void printRoute();
};

#endif // ROUTE_H_INCLUDED