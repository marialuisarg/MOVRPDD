#include "Route.h"
#include "Graph.hpp"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>
#include <tuple>

using namespace std;

Route::Route(double truckCapacity, double droneCapacity, Node* depot) {
    
    truckRoute.push_back(depot);
    
    this->currentTruckCapacity = truckCapacity;
    this->deliveryCost = 0.0;
    this->deliveryTime = 0.0;
    this->energyConsumption = 0.0;

    // add depot at the end of the routes
    truckRoute.push_back(depot);

    this->numClients = 2;
}

Route::~Route() {
    truckRoute.clear();
    droneRoute.clear();
}

void Route::updateDeliveryCost(Graph*g, int CT, int CD, int CB) {
    //cout << "=> updating delivery cost." << endl;

    double droneCost = 0.0;
    double truckCost = 0.0;

    for (int i = 0; i < this->truckRoute.size() - 1; i++)
        truckCost += this->truckRoute[i]->manhattanDistance(this->truckRoute[i + 1]);

    truckCost *= CT;

    for (int j = 0; j < this->droneRoute.size(); j++) {
        double ij = g->getEuclideanDistance(get<0>(this->droneRoute[j]), get<1>(this->droneRoute[j]));
        double jk = g->getEuclideanDistance(get<1>(this->droneRoute[j]), get<2>(this->droneRoute[j]));
        droneCost += (ij + jk);
    }

    droneCost *= CD; 

    //cout << "truck cost: " << truckCost << endl;
    //cout << "drone cost: " << droneCost << endl;
    //cout << "truck + drone cost: " << truckCost + droneCost << endl;
    //cout << "---" << endl;

    setDeliveryCost(truckCost+droneCost+CB);
}

void Route::setDeliveryCost(double cost) {
    this->deliveryCost = cost;
}

void Route::updateEnergyConsumption(Graph *g, int QT) {
    //cout << "=> updating energy consumption." << endl;	
    
    double path = 0.0, ec = 0.0, truckWeight = QT - this->currentTruckCapacity;
    vector<pair<int, double>> launchingNodes;       // <laNode ID, drone's package weight>

    // considering package weight of clients attended by drone (and its lost at the launching node)
    for (int j = 0; j < this->droneRoute.size(); j++) {
        int laNode = get<0>(this->droneRoute[j]);
        double dronePackageWeight = g->getNode(get<1>(this->droneRoute[j]))->getDemand();
        launchingNodes.push_back(make_pair(laNode, dronePackageWeight));
    }

    //cout << this->truckRoute[0]->getID() << " -> " << this->truckRoute[1]->getID() << ": " << truckWeight << endl;

    for (int i = 1; i < this->truckRoute.size() - 1; i++) {
        //cout << endl;
        double dronePackageWeight = 0.0;

        for (int j = 0; j < launchingNodes.size(); j++) {
            if (this->truckRoute[i]->getID() == launchingNodes[j].first) {
                dronePackageWeight = launchingNodes[j].second;
                //cout << "drone is launched at client " << this->truckRoute[i]->getID() << ". drone's client demand is " << launchingNodes[j].second << endl;
                break;
            }
        }

        path = g->getManhattanDistance(this->truckRoute[i]->getID(), this->truckRoute[i+1]->getID());

        if (truckWeight == this->truckRoute[i]->getDemand())
            truckWeight = 0.0;
        else
            truckWeight -= this->truckRoute[i]->getDemand() + dronePackageWeight;

        //cout << this->truckRoute[i]->getID() << " -> " << this->truckRoute[i+1]->getID() << ": " << truckWeight << " (" << this->truckRoute[i]->getID() << "'s demand: " << this->truckRoute[i]->getDemand() << ")" << endl;

        ec += path * truckWeight;
        
        //cout << "energy consumption: " << ec << endl;
    }

    setEnergyConsumption(ec);
    
    //cout << "---" << endl;	
}

void Route::updateDeliveryTime(Graph *g, int VT, int VD) {
    double truckTime = 0.0, droneTime = 0.0;
    setDeliveryTime(0.0);

    // truck time
    for (int j = 0; j < this->truckRoute.size()-1; j++) {
        truckTime += g->getManhattanDistance(this->truckRoute[j]->getID(), this->truckRoute[j+1]->getID()) / VT;
    }

    // drone time (subtracting the time spent by truck during drone's flight)
    for (int i = 0; i < this->droneRoute.size(); i++) {
        double ij = g->getEuclideanDistance(get<0>(this->droneRoute[i]), get<1>(this->droneRoute[i]));
        double jk = g->getEuclideanDistance(get<1>(this->droneRoute[i]), get<2>(this->droneRoute[i]));

        int j;
        for (j = 0; j < this->truckRoute.size(); j++) {
            if (get<0>(this->droneRoute[i]) == this->truckRoute[j]->getID())
                break;
        }

        double aux = 0.0;

        for (j = j; j < this->truckRoute.size()-1; j++) {
            if (get<2>(this->droneRoute[i]) == this->truckRoute[j]->getID())
                break;
            aux += g->getManhattanDistance(this->truckRoute[j]->getID(), this->truckRoute[j+1]->getID()) / VT;
        }

        droneTime += (ij + jk) / VD;
        truckTime -= aux;
    }

    setDeliveryTime(truckTime + droneTime);
}

void Route::setDeliveryTime(double time) {
    this->deliveryTime = time;
}

void Route::updateCapacity(double clientDemand) {
    this->currentTruckCapacity -= clientDemand;
}

void Route::updateNumClients() {
    this->numClients++;
}

double Route::getDeliveryCost() {
    return this->deliveryCost;
}

double Route::getDeliveryTime() {
    return this->deliveryTime;
}

double Route::getEnergyConsumption() {
    return this->energyConsumption;
}

vector<int> Route::getPrevTruckRoute() {
    return this->prevTruckRoute;
}

double Route::getCurrentCapacity() {
    return this->currentTruckCapacity;
}

vector<Node*> Route::getTruckRoute() {
    return this->truckRoute;
}

vector<tuple<int, int, int>> Route::getDroneRoute() {
    return this->droneRoute;
}

int Route::getNumClients() {
    return this->numClients;
}

Node *Route::getNode(int position) {
    return this->truckRoute[position];
}

bool Route::insertClient(Node *client, long int prevNodeIndex) {
    if (client->getDemand() > this->currentTruckCapacity) {
        //cout << "client " << client->getID() << " not inserted. demand: " << client->getDemand() << " | capacity: " << this->currentTruckCapacity << endl;
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

void Route::insertClient(Node *client) {
    this->truckRoute.insert(this->truckRoute.end() - 1, client);
    this->updateCapacity(client->getDemand());
}

void Route::printRoute() {

    cout << "PREV TRUCK ROUTE: ";
    for (int i = 0; i < this->prevTruckRoute.size(); i++) {
        cout << this->prevTruckRoute[i] << " ";
    }

    cout << endl << "CURRENT ROUTES:" << endl;
    for (int i = 0; i < this->truckRoute.size(); i++) {
       cout << this->truckRoute[i]->getID() << " ";
    }

    if (this->droneRoute.size() > 0) {
        cout << "| COST: " << this->deliveryCost;
        cout << " | TRUCK CAPACITY: " << this->currentTruckCapacity;
        cout << endl;
        cout << "DRONE FLIGHTS: ";
    } else {
        cout << "| COST: " << this->deliveryCost;
        cout << " | TRUCK CAPACITY: " << this->currentTruckCapacity;
        cout << endl;
    }

    for (int j = 0; j < this->droneRoute.size(); j++) {
        cout << "(" << get<0>(this->droneRoute[j]) << "," << get<1>(this->droneRoute[j]) << "," << get<2>(this->droneRoute[j]) << ") ";
    }
    cout << endl;
}

Node *Route::getPrevNode(int position) {
    return this->truckRoute[position - 1];
}

Node *Route::getNextNode(int position) {
    return this->truckRoute[position + 1];
}

void Route::insertDroneFlight(tuple<int,int,int> flight) {
    // marks client attended by drone
    get<1>(flight) *= -1;
    this->droneRoute.push_back(flight);
}

void Route::removeClientsServedByDrone(Graph *g, int CT, int CD, int CB) {
    for (int i = 0; i < this->droneRoute.size(); i++) {
        for (int j = 1; j < this->truckRoute.size()-1; j++) {
            if (this->truckRoute[j]->getID() == get<1>(this->droneRoute[i])) {
                this->truckRoute.erase(this->truckRoute.begin() + j);
                break;
            }
        }
    }

    updateDeliveryCost(g, CT, CD, CB);
}

void Route::registerPrevTruckRoute() {
    for (int i = 0; i < this->truckRoute.size(); i++) {
        prevTruckRoute.push_back(truckRoute[i]->getID());
    }
}