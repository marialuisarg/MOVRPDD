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

Population::Population(int size, int numClients) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;
};

Population::~Population() {

};

void Population::include(vector<Solution*> sol) {
    //for (auto it = sol.begin(); it != sol.end(); it++) {
        vector<int> encodedSol = encode(sol[0]);
        //solutions.push_back(encodedSol);
        //currentSize++;
    //}
};

vector<int> Population::encode(Solution *sol) {

    vector<int> encodedSol;
    int numRoutes = sol->getNumRoutes();

    for (int i = 0; i < numRoutes; i++) {
        Route *route = sol->getRoute(i);
        int numNodes = route->getPrevTruckRoute().size()-1;

        for (int j = 0; j < numNodes; j++) 
            encodedSol.push_back(route->getPrevTruckRoute()[j]);
    }

    encodedSol.push_back(0);
    printEncodedSolution(encodedSol);

    return encodedSol;
};

Solution* Population::decode(vector<int> sol) {

};

void Population::printEncodedSolution(vector<int> sol) {
    for (auto it = sol.begin(); it != sol.end(); it++) {
        cout << *it << " ";
    }
};

void Population::printDecodedSolution(Solution *sol) {

};

void Population::printPopulation() {

};