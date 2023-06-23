#include "Solution.h" 
#include "Utils.h"
#include "GreedyConstructor.h"
#include <iostream>

#include <vector>
#include <string>
#include <string.h>
#include <cmath>
#include <fstream>
#include <tuple>

using namespace std;

#define WT 1500 // tare weight of trucks
#define WD 25   // tare weight of drones
#define QD 5    // maximum load capacity of drones
#define CT 25   // travel cost of trucks per unit distance
#define CD 1    // travel cost of drones per unit distance
#define CB 500  // basis cost of using truck equipped with drone
#define E 0.5   // maximum endurance of empty drones
#define ST 60   // average travel speed of trucks
#define SD 65   // average travel speed of drones

#define DEPOT 0
#define DRONE_TRUCK 1
#define TRUCK 2
#define INF 999999999

Solution::Solution(Graph *g, int QT) {
    this->QT = QT;                           // maximum load capacity of trucks
    this->totalDeliveryCost = 0.0;
    this->totalEnergyConsumption = 0.0;
    this->totalDeliveryTime = 0.0;
    this->drone = false;

    // calculates demand and divides by max load capacity of trucks to get num of routes
    setNumRoutes(ceil(g->getTotalDemand() / QT));
}

void Solution::updateSolution(Graph *g) {
    double f1 = 0.0, f2 = 0.0, f3 = 0.0;

    for (int i = 0; i < getNumRoutes(); i++) {
        routes[i].updateEnergyConsumption(g, QT);
        routes[i].updateDeliveryTime(g, ST, SD);

        f1 += routes[i].getEnergyConsumption();
        f2 += routes[i].getDeliveryCost();
        if (routes[i].getDeliveryTime() > f3)
            f3 = routes[i].getDeliveryTime();
    }

    this->totalEnergyConsumption = f1;
    this->totalDeliveryCost = f2;
    this->totalDeliveryTime = f3;
}

void Solution::printRoutes() {
    cout << endl;
    for (int i = 0; i < getNumRoutes(); i++) {
        cout << "Route " << i << ": ";
        routes[i].printRoute();
    }
}

void Solution::plotSolution(string instance, int i){
    // plot solution
    string instanceName = instance;
    string filename;
    
    if (droneIsUsed())
        filename = "./solutions/droneSolution" + to_string(i) + "_" + instance.erase(0,12);
    else
        filename = "./solutions/truckSolution" + to_string(i) + "_" + instance.erase(0,12);
    
    ofstream output_file(filename);

    int nRoutes = getNumRoutes();
    double totalEnergyConsumption = getTotalEnergyConsumption();
    double totalDeliveryCost = getTotalDeliveryCost();
    double totalDeliveryTime = getTotalDeliveryTime();

    output_file << nRoutes << endl;
    output_file << totalEnergyConsumption << endl;
    output_file << totalDeliveryCost << endl;
    output_file << totalDeliveryTime << endl;

    for (int i=0; i < nRoutes; i++) {
        string truckRoute = "";

        for (int j=0; j < this->routes[i].getTruckRoute().size(); j++){
            truckRoute = truckRoute + "-" + to_string(this->routes[i].getTruckRoute()[j]->getID());
        }

        if(!truckRoute.empty())
            output_file << truckRoute.substr(1);
        output_file << endl;

        string droneRoute = " ";

        for (int j=0; j < this->routes[i].getDroneRoute().size(); j++){
            droneRoute = droneRoute + "/" + to_string(get<0>(this->routes[i].getDroneRoute()[j]))
                                    + "-" + to_string(get<1>(this->routes[i].getDroneRoute()[j]))
                                    + "-" + to_string(get<2>(this->routes[i].getDroneRoute()[j]));
        }

        if(!droneRoute.empty())
            output_file << droneRoute.substr(1);
        output_file << endl;
    }

    output_file.close();
    
    string command = "python plotSolution.py " + instanceName + " " + filename;
    int aux = system(command.c_str());
    // command = "rm " + filename;
    // aux = system(command.c_str());
}

vector<Route> Solution::getRoutes() {
    return routes;
}

double Solution::getTotalDeliveryCost() {
    return this->totalDeliveryCost;
}

double Solution::getTotalEnergyConsumption() {
    return this->totalEnergyConsumption;
}

double Solution::getTotalDeliveryTime() {
    return this->totalDeliveryTime;
}

Solution::~Solution() {
    routes.clear();
}