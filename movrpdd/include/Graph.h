#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

using namespace std;

#include "Node.h"
#include <vector>

class Graph {
    private:
        vector<Node> Nodes;
        vector<vector<double>> euclideanDistances;
        vector<vector<double>> manhattanDistances;
         int size;

    public:
        Graph(int size);
        ~Graph();
        void addNode(Node node);
        void addEdge( int from,  int to);
        void printGraph();
        void printEuclideanDistances();
        void printManhattanDistances();
        double getEuclideanDistance( int from,  int to);
        double getManhattanDistance( int from,  int to);
        void clear();
         int getSize();
        double getTotalDemand();
        vector<Node> getNodes();
        Node* getNode(int index);
};

#endif // GRAPH_H_INCLUDED