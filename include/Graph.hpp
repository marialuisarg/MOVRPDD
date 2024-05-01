#ifndef ENSGA2_HPP_
#define ENSGA2_HPP_

#include "Node.h"

#include <iostream>
#include <vector>

class Graph {

    vector<Node>            Nodes;                  // vector of client nodes
    vector<vector<double>>  euclideanDistances;     // euclidean distance matrix
    vector<vector<double>>  manhattanDistances;     // manhattan distance matrix
    int                     size;                   // number of nodes in the graph

    Graph(int graphSize) {
        this->size = graphSize;
        euclideanDistances.resize(graphSize, vector<double>(graphSize));
        manhattanDistances.resize(graphSize, vector<double>(graphSize));
    };

    ~Graph();

    void addNode(Node node) {    Nodes.push_back(node);  };

    void addEdge(int from, int to) {
        euclideanDistances[from][to] = Nodes[from].euclideanDistance(&Nodes[to]);
        manhattanDistances[from][to] = Nodes[from].manhattanDistance(&Nodes[to]);
    };
    
    double getEuclideanDistance(int from,  int to) {    return this->euclideanDistances[from][to];  };

    double getManhattanDistance(int from,  int to) {    return this->manhattanDistances[from][to];  };

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