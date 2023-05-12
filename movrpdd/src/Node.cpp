#include "Node.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>

using namespace std;

Node::Node(int clientID, char type, double x, double y, double demand, int serviceBy) {
    ID = clientID;
    Type = type;
    X = x;
    Y = y;
    Demand = demand;
    ServiceBy = serviceBy;
}

Node::~Node() {
}

int Node::getID() const {
    return ID;
}

char Node::getType() const {
    return Type;
}

double Node::getX() const {
    return X;
}

double Node::getY() const {
    return Y;
}

double Node::getDemand() const {
    return Demand;
}

int Node::getServiceBy() const {
    return ServiceBy;
}

void Node::printNode() {
    cout << ID << "\t";
    cout << Type << "\t";
    cout << X << "\t";
    cout << Y << "\t";
    cout << Demand << "\t";
    
    switch (ServiceBy) {
        case 0:
            cout << "-";
            break;
        case 1:
            cout << "D/T";
            break;
        case 2:
            cout << "T";
            break;
        default:
            cout << "Error";
            break;
    }

    cout << endl;
}

double Node::euclideanDistance(Node *node) {
    if (getX() == node->getX() && getY() == node->getY()) {
        return 0;
    }
    else {
        return sqrt(pow((getX() - node->getX()), 2) + pow((getY() - node->getY()), 2));
    }
}

double Node::manhattanDistance(Node *node) {
    if (getX() == node->getX() && getY() == node->getY()){
        return 0;
    }
    else {
        return abs(getX() - node->getX()) + abs(getY() - node->getY());
    }
}