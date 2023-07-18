#include "Solution.h"
#include "GreedyConstructor.h"
#include "RandomConstructor.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <tuple>

using namespace std;

// parameter settings of MOVRPDD Model //
 
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

vector<pair<int, bool>> aux_attendedClients;
vector<tuple<int, int, double, int, int>> aux_candidatesCost;

Solution * RandomConstructor(Graph *g, int QT, double alpha, int numIterations) {
    int numRoutes;
    bool droneRouteCreated;

    Solution * bestSolution = new Solution(g, QT);
    Solution * currentSolution = new Solution(g, QT);

    int n = 0;

    while (n < numIterations) {
        for (int i = 0; i < g->getSize(); i++) {
            aux_attendedClients.push_back({g->getNode(i)->getID(), false});  
        }

        // depot visited 
        aux_attendedClients[0].second = true;

        // creates routes
        for (int i = 0; i < currentSolution->getNumRoutes(); i++) {
            Route r(QT, QD, g->getNode(0));
            currentSolution->createRoute(r);
        }

        // creates truck routes
        createRandomTruckRoutes(g, currentSolution, &numRoutes, &droneRouteCreated, alpha);
        if (currentSolution->isBetterThan(bestSolution))
            bestSolution = currentSolution;
        n++;
    }

    return bestSolution;
}

void createRandomTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, double alpha) {

    // creates list of candidates
    vector<Node*> candidates;

    // inserts unattended clients into the candidate list
    for (int i = 0; i < g->getSize(); i++) {
        if (!aux_attendedClients[i].second) {
            candidates.push_back(g->getNode(i));
        }
    }

    // calculates cost of each candidate
    for (int r = 0; r < sol->getNumRoutes(); r++) {
        for (int i = 0; i < candidates.size(); i++) {
            double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
            double cost = manhattanDistance * CT * 2;
            aux_candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost,0,0));
        }
    }

    // sorts candidates by cost
    sortCandidatesByCost(g, sol);

    // while there are unattended clients
    while (!allClientsAttended(g, sol)) {

        // gets random index for client
        int k = rand () % (int) (alpha * aux_candidatesCost.size()-1);

        int clientID = get<0>(aux_candidatesCost[k]);
        int routeIndex = get<1>(aux_candidatesCost[k]);
        int prevNode = get<3>(aux_candidatesCost[k]);

        Node *client = g->getNode(clientID);

        if (includeClient(client, sol, sol->getRoute(routeIndex), g, prevNode, routeIndex)) {
            cout << "Random client " << client->getID() << " inserted in route " << routeIndex << endl;
        }
    }

    // register final truck routes (before drones)
    for (int i = 0; i < sol->getNumRoutes(); i++) 
        sol->getRoute(i)->registerPrevTruckRoute();

    //printRoutes();
    sol->updateSolution(g);
}