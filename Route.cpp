#include "Route.h"
#include "Graph.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>
#include <tuple>

using namespace std;

Route::Route(double truckCapacity, double droneCapacity, Node* depot) {
    
    truckRoute.push_back(depot);
    
    this->currentTruckCapacity = truckCapacity;
    this->deliveryCost.first = 0.0;
    this->deliveryCost.second = 0.0;
    this->deliveryTime.first = 0.0;
    this->deliveryTime.second = 0.0;
    this->energyConsumption.first = 0.0;
    this->energyConsumption.second = 0.0;

    // add depot at the end of the routes
    truckRoute.push_back(depot);

    this->numClients = 2;
}

Route::~Route() {
    truckRoute.clear();
    droneRoute.clear();
}

void Route::updateDeliveryCost(Graph*g, int CT, int CD, int CB, int typeOfRoute) {
    cout << "=> updating delivery cost." << endl;

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

    cout << "truck cost: " << truckCost << endl;
    cout << "drone cost: " << droneCost << endl;
    cout << "truck + drone cost: " << truckCost + droneCost << endl;
    cout << "---" << endl;

    if (typeOfRoute == TRUCK_DRONE)
        setDeliveryCost(truckCost+droneCost+CB, typeOfRoute);
    else if (typeOfRoute == TRUCK)
        setDeliveryCost(truckCost+CB, typeOfRoute);
}

void Route::setDeliveryCost(double cost, int typeOfRoute) {
    if (typeOfRoute == TRUCK_DRONE)
        this->deliveryCost.second = cost;
    else if (typeOfRoute == TRUCK)
        this->deliveryCost.first = cost;
}

void Route::updateEnergyConsumption(Graph *g, int typeOfRoute, int QT) {
    //cout << "=> updating energy consumption." << endl;	
    
    double path, ec, truckWeight = QT - this->currentTruckCapacity;
    this->energyConsumption = make_pair(0.0, 0.0);
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

    if (typeOfRoute == TRUCK_DRONE)
        this->energyConsumption.second = ec;
    else if (typeOfRoute == TRUCK)
        this->energyConsumption.first = ec;
    
    cout << "---" << endl;	
}

void Route::updateDeliveryTime(Graph *g, int typeOfRoute, int VT, int VD) {
    double truckTime = 0.0, droneTime = 0.0;
    this->deliveryTime = make_pair(0.0, 0.0);

    // truck time
    for (int j = 0; j < this->truckRoute.size()-1; j++) {
        truckTime += g->getManhattanDistance(this->truckRoute[j]->getID(), this->truckRoute[j+1]->getID()) / VT;
    }

    // drone time (subtracting the time spent by truck during drone's flight)
    for (int i = 0; i < this->droneRoute.size(); i++) {
        double ij = g->getEuclideanDistance(get<0>(this->droneRoute[i]), get<1>(this->droneRoute[i]));
        double jk = g->getEuclideanDistance(get<1>(this->droneRoute[i]), get<2>(this->droneRoute[i]));

        int index=0, nodeID=0;
        while (nodeID != get<0>(this->droneRoute[i])) {
            index++;
            nodeID = this->truckRoute[index]->getID();
        }
        double aux = 0.0;
        while (nodeID != get<2>(this->droneRoute[i])) {
            aux += g->getManhattanDistance(this->truckRoute[index]->getID(), this->truckRoute[index+1]->getID()) / VT;
            index++;
            nodeID = this->truckRoute[index]->getID();
        }

        droneTime += (ij + jk) / VD;
        truckTime -= aux;
    }

    if (typeOfRoute == TRUCK_DRONE)
        this->deliveryTime.second = truckTime + droneTime;
    else if (typeOfRoute == TRUCK)
        this->deliveryTime.first = truckTime;
}

void Route::updateCapacity(double clientDemand) {
    this->currentTruckCapacity -= clientDemand;
}

void Route::updateNumClients() {
    this->numClients++;
}

double Route::getDeliveryCost(int typeOfRoute) {
    if (typeOfRoute == TRUCK)
        return this->deliveryCost.first;
    else
        return this->deliveryCost.second;
}

double Route::getDeliveryTime(int typeOfRoute) {
    if (typeOfRoute == TRUCK)
        return this->deliveryTime.first;
    else
        return this->deliveryTime.second;
}

double Route::getEnergyConsumption(int typeOfRoute) {
    if (typeOfRoute == TRUCK)
        return this->energyConsumption.first;
    else
        return this->energyConsumption.second;
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
    cout << endl;
    for (int i = 0; i < this->truckRoute.size(); i++) {
        cout << this->truckRoute[i]->getID() << " ";
    }

    if (this->droneRoute.size() > 0) {
        cout << "| COST: " << this->deliveryCost.second;
        cout << " | TRUCK CAPACITY: " << this->currentTruckCapacity;
        cout << endl;
        cout << "DRONE FLIGHTS: ";
    } else {
        cout << "| COST: " << this->deliveryCost.first;
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
    //cout << "drone flight inserted!" << endl;
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

    updateDeliveryCost(g, CT, CD, CB, TRUCK_DRONE);
}

void Route::registerPrevTruckRoute() {
    for (int i = 0; i < this->truckRoute.size(); i++) {
        prevTruckRoute.push_back(truckRoute[i]->getID());
    }
}