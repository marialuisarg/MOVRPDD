#include "Route.h"
#include "Graph.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>

using namespace std;

Route::Route(double truckCapacity, double droneCapacity, Node* depot) {
    
    truckRoute.push_back(depot);
    
    this->currentTruckCapacity = truckCapacity;
    this->cost = 0.0;

    // add depot at the end of the routes
    truckRoute.push_back(depot);

    this->numClients = 2;
}

Route::~Route() {
    truckRoute.clear();
    //droneRoute.clear();
}

void Route::updateCost(int CT) {
    this->cost = 0.0;
    for (int i = 0; i < this->truckRoute.size() - 1; i++) {
        //cout << this->truckRoute[i]->getID() << " => " << this->truckRoute[i + 1]->getID() << " (" << this->truckRoute[i]->manhattanDistance(this->truckRoute[i + 1]) * CT << ")" << endl;
        this->cost += this->truckRoute[i]->manhattanDistance(this->truckRoute[i + 1]);
    }
    this->cost *= CT;
}

void Route::updateCapacity(double clientDemand) {
    this->currentTruckCapacity -= clientDemand;
}

void Route::updateNumClients() {
    this->numClients++;
}

double Route::getCost() {
    return this->cost;
}

double Route::getCurrentCapacity() {
    return this->currentTruckCapacity;
}

vector<Node*> Route::getTruckRoute() {
    return this->truckRoute;
}

// vector<tuple<int, int, int>> Route::getDroneRoute() {
//     return this->droneRoute;
// }

int Route::getNumClients() {
    return this->numClients;
}

Node *Route::getNode(int position) {
    return this->truckRoute[position];
}

bool Route::insertClient(Node *client, int prevNodeIndex) {
    if (client->getDemand() > this->currentTruckCapacity) {
        return false;
    }

    for (int i = 0; i < this->truckRoute.size(); i++) {
        if (this->truckRoute[i]->getID() == prevNodeIndex) {
            this->truckRoute.insert(this->truckRoute.begin() + i + 1, client);
            this->updateCapacity(client->getDemand());
            break;
        }
    }
    return true;
}

void Route::printRoute() {
    for (int i = 0; i < this->truckRoute.size(); i++) {
        cout << this->truckRoute[i]->getID() << " ";
    }
    cout << "| COST: " << this->cost;
    cout << " | TRUCK CAPACITY: " << this->currentTruckCapacity;
    cout << endl;
}

Node *Route::getPrevNode(int position) {
    return this->truckRoute[position - 1];
}

Node *Route::getNextNode(int position) {
    return this->truckRoute[position + 1];
}