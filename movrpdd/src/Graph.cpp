#include "Graph.h"
#include "Node.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>

using namespace std;

Graph::Graph(int graphSize) {
    this->size = graphSize;
    euclideanDistances.resize(graphSize, vector<double>(graphSize));
    manhattanDistances.resize(graphSize, vector<double>(graphSize));
}

Graph::~Graph() {
}

void Graph::addNode(Node node) {
    Nodes.push_back(node);
}

void Graph::addEdge( int from,  int to) {
    euclideanDistances[from][to] = Nodes[from].euclideanDistance(&Nodes[to]);
    manhattanDistances[from][to] = Nodes[from].manhattanDistance(&Nodes[to]);
}

void Graph::printGraph() {

    cout << "ID\tType\tX\tY\tDemand\tServiceBy" << endl;

    for ( int i = 0; i < getSize(); i++) {
        Nodes[i].printNode();
    }
}

void Graph::printEuclideanDistances() {

    cout << endl << "Euclidean Distances:" << endl;

    for ( int i = 0; i < getSize(); i++) {
        for ( int j = 0; j < getSize(); j++) {
            // cout << "i = " << i << " j = " << j << endl;
            cout << this->euclideanDistances[i][j] << " ";
        }
        cout << endl;
    }
}

void Graph::printManhattanDistances() {

    cout << endl << "Manhattan Distances:" << endl;

    for (int i = 0; i < getSize(); i++) {
        for (int j = 0; j < getSize(); j++) {
            // cout << "i = " << i << " j = " << j << endl;
            cout << this->manhattanDistances[i][j] << " ";
        }
        cout << endl;
    }
}

void Graph::clear() {
    Nodes.clear();
    euclideanDistances.clear();
    manhattanDistances.clear();
    size = 0;
}

 int Graph::getSize() {
    return this->size;
}

double Graph::getEuclideanDistance( int from,  int to) {
    return this->euclideanDistances[from][to];
}

double Graph::getManhattanDistance( int from,  int to) {
    return this->manhattanDistances[from][to];
}

vector<Node> Graph::getNodes() {
    return this->Nodes;
}

Node* Graph::getNode(int index) {
    return &Nodes[index];
}

double Graph::getTotalDemand() {
    double totalDemand = 0;
    for (int i = 0; i < getSize(); i++) {
        totalDemand += Nodes[i].getDemand();
    }
    return totalDemand;
}