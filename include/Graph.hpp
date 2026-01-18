#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include "Node.hpp"

#include <iostream>
#include <vector>

class Graph {

    private:
        vector<Node>            Nodes;                  // vector of client nodes
        vector<vector<double>>  euclideanDistances;     // euclidean distance matrix
        vector<vector<double>>  manhattanDistances;     // manhattan distance matrix
        vector<vector<double>>  truckTravelTimes;       // truck travel time matrix
        vector<vector<double>>  droneFlightEndurance;   // drone loading rate matrix adjusted to consider flight endurance
        vector<vector<double>>  droneFlightTimes;       // drone flight time matrix
        int                     size;                   // number of nodes in the graph

    public:
        Graph(int graphSize) {
            this->size = graphSize;
            euclideanDistances.resize(graphSize, vector<double>(graphSize));
            manhattanDistances.resize(graphSize, vector<double>(graphSize));
            truckTravelTimes.resize(graphSize, vector<double>(graphSize));
            droneFlightEndurance.resize(graphSize, vector<double>(graphSize));
            droneFlightTimes.resize(graphSize, vector<double>(graphSize));
        };

        ~Graph(){};

        void addNode(Node node) {    Nodes.push_back(node);  };

        void addEdge(int from, int to) {
            euclideanDistances[from][to] = Nodes[from].euclideanDistance(&Nodes[to]);
            manhattanDistances[from][to] = Nodes[from].manhattanDistance(&Nodes[to]);
            truckTravelTimes[from][to] = manhattanDistances[from][to] / ST;
            
            if (Nodes[from].getType() == DEPOT || Nodes[to].getType() == DEPOT || Nodes[to].getType() == TRUCK) {
                droneFlightEndurance[from][to] = 0; // no drone loading rate between depot and clients and no drone loading rate when client is only served by truck
                droneFlightTimes[from][to] = INF;
            } else {
                droneFlightEndurance[from][to] = 1 - (0.2 * (Nodes[from].getDemand() / QD)); //  dynamic scaling factor for the maximum flight endurance of the loaded drones
                droneFlightTimes[from][to] = (euclideanDistances[from][to] / SD);            // time to fly from one client to another
            }
        };
        
        double getEuclideanDistance(int from,  int to) {    return this->euclideanDistances[from][to];  };

        double getManhattanDistance(int from,  int to) {    return this->manhattanDistances[from][to];  };

        double getDroneFlightEndurance(int from, int to) {    return this->droneFlightEndurance[from][to];  };

        double getDroneFlightTime(int from, int to) {    return this->droneFlightTimes[from][to];  };

        double getTruckTravelTime(int from, int to) {   return this->truckTravelTimes[from][to];    };

        int getSize() { return this->size;  };

        double getTotalDemand() {
            double totalDemand = 0;
            for (int i = 0; i < getSize(); i++)
                totalDemand += Nodes[i].getDemand();
            return totalDemand;
        };

        vector<Node> getNodes() {    return Nodes;  };
        
        Node* getNode(int index) {    return &Nodes[index];  };
        
        void printGraph() {
            std::cout << "StringID\tID\tType\tX\tY\tDemand\tServiceBy" << std::endl;
            std::cout << "StringID\tID\tType\tX\tY\tDemand\tServiceBy" << std::endl;

            for ( int i = 0; i < getSize(); i++) {
                Nodes[i].printNode();
            }
        };

        void printEuclideanDistances() {
            std::cout << endl << "Euclidean Distances:" << std::endl;

            for ( int i = 0; i < getSize(); i++) {
                for ( int j = 0; j < getSize(); j++) {
                    // cout << "i = " << i << " j = " << j << endl;
                    std::cout << this->euclideanDistances[i][j] << " ";
                }
                std::cout << std::endl;
            }
        };

        void printManhattanDistances() {
            std::cout << endl << "Manhattan Distances:" << std::endl;

            for (int i = 0; i < getSize(); i++) {
                for (int j = 0; j < getSize(); j++) {
                    // cout << "i = " << i << " j = " << j << endl;
                    std::cout << this->manhattanDistances[i][j] << " ";
                }
                std::cout << std::endl;
            }
        };
        
        void clear() {
            Nodes.clear();
            euclideanDistances.clear();
            manhattanDistances.clear();
            size = 0;
        };
};

#endif /* GRAPH_HPP_ */