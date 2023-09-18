#include "Population.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <tuple>

using namespace std;

#define DEPOT 0
#define DRONE_TRUCK 1
#define TRUCK 2
#define INF 999999999

#define QT 1000
#define QD 5
#define CB 500  // basis cost of using truck equipped with drone
#define ST 60   // average travel speed of trucks
#define SD 65   // average travel speed of drones

Population::Population(int size, int numClients, Graph *g, int q) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;
};

Population::~Population() {

};

void Population::include(vector<Solution*> sol, Graph *g) {
    //for (auto it = sol.begin(); it != sol.end(); it++) {
        vector<int> encodedSol = sol[0]->encode();
        //solutions.push_back(encodedSol);
        //currentSize++;
    //}

    Solution *s = decode(encodedSol, g, QT);
};

Solution* Population::decode(vector<int> sol, Graph *g, int q) {

    cout << endl << "DECODED SOLUTION " << endl << "-------------------" << endl;

    Solution *decodedSol = new Solution(g, QT);
    int numRoutes = decodedSol->getNumRoutes();

    // SPLITTING ENCODED SOLUTION INTO ROUTES
    vector<int> currentRoute;
    vector<vector<int>> routes;
    
    for (int num : sol) {
        if (num == 0) {
            if (!currentRoute.empty()) {
                routes.push_back(currentRoute);
                currentRoute.clear();
            }
        } else {
            currentRoute.push_back(num);
        }
    }

    if (!currentRoute.empty()) {
        routes.push_back(currentRoute);
    }

    // PASSING ROUTES TO SOLUTION
    double f1 = 0.0, f2 = 0.0, f3 = 0.0;

    for (const auto& route : routes) {
        Route *r = new Route(QT, QD, g->getNode(0));
        Node *currentNode;

        decodedSol->createRoute(*r);

        for (int num : route) {
            currentNode = g->getNode(num);
            r->insertClient(currentNode);
        }

        cout << endl;

        r->updateEnergyConsumption(g, QT);
        r->updateDeliveryTime(g, ST, SD);
        r->updateDeliveryCost(g, QT, QD, CB);

        f1 += r->getEnergyConsumption();
        f2 += r->getDeliveryCost();
        if (r->getDeliveryTime() > f3)
            f3 = r->getDeliveryTime();

        r->registerPrevTruckRoute();
        r->printRoute();
    }

    decodedSol->setTotalEnergyConsumption(f1);
    decodedSol->setTotalDeliveryCost(f2);
    decodedSol->setTotalDeliveryTime(f3);

    // TODO: CREATE DRONE ROUTES
    //createDroneRoutes(g, decodedSol);
    decodedSol->printRoutes();

    return decodedSol;
};

void Population::printDecodedSolution(Solution *sol) {

};

void Population::printPopulation() {

};