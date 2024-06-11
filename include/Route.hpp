#ifndef ROUTE_H_INCLUDED
#define ROUTE_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Node.hpp"
#include "Graph.hpp"

class Route {
    private:
        vector<Node*> truckRoute;
        vector<int> prevTruckRoute;                 // final truck route before drone flights
        vector<tuple<int, int, int>> droneRoute;    // launch node, client, retrieval node 
    
        double energyConsumption;      // f1
        double deliveryCost;           // f2
        double deliveryTime;           // f3

        double currentTruckCapacity;
        double currentDroneCapacity;
        int numClients;
        
    public:
        Route(double truckCapacity, double droneCapacity, Node* depot);
        ~Route();

        void updateDeliveryCost(Graph *g, int CT, int CD, int CB);
        void updateEnergyConsumption(Graph *g, int QT);
        void updateDeliveryTime(Graph *g, int VT, int VD);
        void updateCapacity(double capacity);
        void updateNumClients();

        void removeClientsServedByDrone(Graph *g, int CT, int CD, int CB);
        void registerPrevTruckRoute();

        double getDeliveryCost();
        double getEnergyConsumption();
        double getDeliveryTime();
        double getPrevCost();
        double getCurrentCapacity();
        vector<Node*> getTruckRoute();
        vector<int> getPrevTruckRoute();
        vector<tuple<int, int, int>> getDroneRoute();
        int getNumClients();
        Node* getNode(int position);
        Node* getPrevNode(int position);
        Node* getNextNode(int position);

        void insertClient(Node *client);
        bool insertClient(Node *client, long int prevNode);  // returns false if client can't be inserted
        void insertDroneFlight(tuple<int,int,int> flight);

        void printRoute();

        void setDeliveryCost(double cost);
        void setDeliveryTime(double time);
        void setEnergyConsumption(double ec) { this->energyConsumption = ec; };
};

#endif // ROUTE_H_INCLUDED