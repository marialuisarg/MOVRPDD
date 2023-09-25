#include "Population.h"
#include "RandomConstructor.h"
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

#define RANDOM_GREEDY 1

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

// starts population with constructor
Population::Population(int size, int numClients, Graph *g, int q, double alpha, int numIterations, int constructorType) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;

    vector<Solution*> sol = RandomConstructor(g, q, alpha, numIterations, size);
    include(sol, g);
};

Population::~Population() {
};

void Population::include(vector<Solution*> sol, Graph *g) {
    for (auto it = sol.begin(); it != sol.end(); it++) {
        vector<int> encodedSol = (*it)->encode();
        solutions.push_back(encodedSol);
        currentSize++;
    }
};

vector<vector<int>> Population::getSolutions() {
    return solutions;
};

Solution* Population::decode(vector<int> sol, Graph *g, int q) {

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

    // creates routes
    for (int i = 0; i < routes.size(); i++) {
        Route r(QT, QD, g->getNode(0));
        decodedSol->createRoute(r);
    }

    int index = 0;

    for (const auto& route : routes) {
        Node *currentNode;
        Route *r = decodedSol->getRoute(index);

        for (int num : route) {
            currentNode = g->getNode(num);
            r->insertClient(currentNode);
        }

        r->updateEnergyConsumption(g, QT);
        r->updateDeliveryTime(g, ST, SD);
        r->updateDeliveryCost(g, QT, QD, CB);

        f1 += r->getEnergyConsumption();
        f2 += r->getDeliveryCost();
        if (r->getDeliveryTime() > f3)
            f3 = r->getDeliveryTime();

        r->registerPrevTruckRoute();
        index++;
    }

    decodedSol->setTotalEnergyConsumption(f1);
    decodedSol->setTotalDeliveryCost(f2);
    decodedSol->setTotalDeliveryTime(f3);

    // CREATE DRONE ROUTES
    createDroneRoutes(g, decodedSol);

    return decodedSol;
};

void Population::FNDS(Graph *g) {
    // fast non-dominated sort (Deb, 2002)

    for (const auto& solution : solutions) {
        Solution *sol = decode(solution, g, QT);
        int np = 0;                                 // number of solutions that dominate p
        vector<vector<int>> sp;                     // set of solutions that p dominates

        for (const auto& solution2 : solutions) {
            Solution *sol2 = decode(solution2, g, QT);

            if (sol->dominates(sol2)) 
                sp.push_back(solution2);
            else if (sol2->dominates(sol))
                np++;
        }

        if (np == 0) {
            sol->setRank(1);
            fronts[0].push_back(solution);
        }
    }

    int i = 1;                  // front counter
    int n = solutions.size();   // number of solutions

}

void Population::printDecodedSolution(Solution *sol) {
    cout << "DECODED SOLUTION FUNCTIONS: " << endl;
    cout << "f1: " << sol->getTotalEnergyConsumption();
    cout << " | f2: " << sol->getTotalDeliveryCost();
    cout << " | f3: " << sol->getTotalDeliveryTime() << endl;
};