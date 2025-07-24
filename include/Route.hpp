#ifndef ROUTE_H_INCLUDED
#define ROUTE_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "Node.hpp"
#include "Graph.hpp"

class Route {
    private:
        std::vector<Node*> truckRoute;
        std::vector<int> truckRouteIDs;                  // final truck route before drone flights
        std::unordered_map<int, int> serviceType;        // 0: truck, 1: drone
        std::vector<tuple<int, int, int>> droneRoute;    // launch node, client, retrieval node 
    
        double energyConsumption;      // f1
        double deliveryCost;           // f2
        double deliveryTime;           // f3

        double currentTruckCapacity;
        double currentDroneCapacity;

        double currentTruckRouteCost;
        double currentDroneRouteCost;

           int numClients;
        
    public:
        Route(double truckCapacity, double droneCapacity, Node* depot);
        ~Route();

        void setDeliveryCost(Graph *g, int CT, int CD, int CB);
        void updateDeliveryCost(Graph *g, int CT, int CD, int CB, std::vector<tuple<int, int, int>> flightIndexes);
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
        std::vector<Node*> getTruckRoute();
        std::vector<int>* getTruckRouteIDs() {    return &this->truckRouteIDs;      };
        std::vector<std::tuple<int, int, int>> getDroneRoute();
        int getNumClients();
        Node* getNode(int position);
        Node* getPrevNode(int position);
        Node* getNextNode(int position);
        bool isClientServedByDrone(int clientID);

        void insertClient(Node *client, int ID);
        bool insertClientPrev(Node *client, long int prevNode);  // returns false if client can't be inserted
        void insertDroneFlight(std::tuple<int,int,int> flight);

        void printRoute();

        void setDeliveryCost(double cost);
        void setDeliveryTime(double time);
        void setEnergyConsumption(double ec) { this->energyConsumption = ec; };
};

#endif // ROUTE_H_INCLUDED