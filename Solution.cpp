#include "Solution.h" 
#include "Utils.h"
#include <iostream>

#include <vector>
#include <string>
#include <string.h>
#include <cmath>
#include <fstream>
#include <tuple>

// parameter settings of MOVRPDD Model //
 
#define WT 1500 // tare weight of trucks 
#define QT 25 // maximum load capacity of trucks
#define WD 25   // tare weight of drones
#define QD 5    // maximum load capacity of drones
#define CT 1    // travel cost of trucks per unit distance
#define CD 1    // travel cost of drones per unit distance
#define CB 500  // basis cost of using truck equipped with drone
#define E 0.5   // maximum endurance of empty drones
#define ST 60   // average travel speed of trucks
#define SD 65   // average travel speed of drones

#define DEPOT 0
#define DRONE_TRUCK 1
#define TRUCK 2
#define INF 999999999

using namespace std;

Solution::Solution(Graph *g) {
    this->cost = 0.0;

    for (int i = 0; i < g->getSize(); i++) {
        attendedClients.push_back({g->getNode(i)->getID(), false});
    }

    // depot visited 
    attendedClients[0].second = true;

    // calculates demand and divides by max load capacity of trucks to get num of routes
    int numRoutes = ceil(g->getTotalDemand() / QT);

    // creates routes
    for (int i = 0; i < numRoutes; i++) {
        Route r(QT, QD, g->getNode(0));
        routes.push_back(r);
    }

    createTruckRoutes(g);
    updateSolutionCost();
    //createDroneRoutes(g);
}

void Solution::updateSolutionCost() {
    double c = 0.0;
    for (int i = 0; i < routes.size(); i++) {
        c += routes[i].getCost();
    }

    this->cost = c;
}

void Solution::createTruckRoutes(Graph *g) {

    // creates list of candidates
    vector<Node*> candidates;

    // inserts unattended clients into the candidate list
    for (int i = 0; i < g->getSize(); i++) {
        if (!attendedClients[i].second) {
            candidates.push_back(g->getNode(i));
        }
    }

    // calculates cost of each candidate
    for (int r = 0; r < routes.size(); r++) {
        for (int i = 0; i < candidates.size(); i++) {
            double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
            double cost = manhattanDistance * CT;
            candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost,0,0));
        }
    }

    // sorts candidates by cost
    sortCandidatesByCost(g);

    cout << endl;

    printRoutes();
    printCandidatesCost();

    // while there are unattended clients
    while (!allClientsAttended(g)) {

        // gets cheapest insertion
        Node *cheapestInsertion = g->getNode(get<0>(candidatesCost[0]));

        // gets route index
        int iRoute = get<1>(candidatesCost[0]);

        // gets previous node index in route
        int prevNode = get<3>(candidatesCost[0]);

        // inserts client in route ("if" logic here is just for debugging, but function must be called)
        if (includeClient(cheapestInsertion, &routes[iRoute], g, prevNode, iRoute))
            printRoutes();
        
        printCandidatesCost();
        cout << endl;
    }

    printRoutes();
}

void Solution::sortCandidatesByCost(Graph *g) {
    int n = candidatesCost.size();
    tuple<int, int, double, int, int> temp;
    
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (get<2>(candidatesCost.at(j)) > get<2>(candidatesCost.at(j + 1))) {
                temp = candidatesCost.at(j);
                candidatesCost.at(j) = candidatesCost.at(j + 1);
                candidatesCost.at(j + 1) = temp;
            }
        }
    }
}

bool Solution::includeClient(Node *client, Route *r, Graph *g, int prevNodeIndex, int iRoute) {

    // inserts client in route
    if (!r->insertClient(client, prevNodeIndex)) {
        // if client can't be inserted, removes it from candidates list
        cout << "Client " << client->getID() << " can't be inserted in route " << iRoute << endl;
        cout << endl;
        for (int i = 0; i < candidatesCost.size(); i++) {
            if (get<0>(candidatesCost[i]) == client->getID() && get<1>(candidatesCost[i]) == iRoute) {
                candidatesCost.erase(candidatesCost.begin()+i);
                i--;
            }
        }

        return false;
    }

    // updates attended clients
    updateAttendedClients(client->getID());
    r->updateCost(CT);
    
    // updates cheapest insertion candidates list
    updateCandidatesList(client, r, g, iRoute);
    return true;
}

bool verifyNeighbor(Route *r, int currentPrevIndex, int currentNextIndex) {
    for (int j = 0; j < r->getTruckRoute().size()-1; j++)
        if (r->getTruckRoute()[j]->getID() == currentPrevIndex)
            if (r->getTruckRoute()[j+1]->getID() != currentNextIndex)
                return false;
    return true;
}

void Solution::updateCandidatesList(Node *client, Route *r, Graph *g, int iRoute) {
   
   int clientPrevNode, clientNextNode;

   // remove client from candidates list
    for (int i = 0; i < candidatesCost.size(); i++) {
        if (get<0>(candidatesCost[i]) == client->getID()) {
            if (get<1>(candidatesCost[i]) == iRoute) {
                clientPrevNode = get<3>(candidatesCost[i]);
                clientNextNode = get<4>(candidatesCost[i]);
            }

            candidatesCost.erase(candidatesCost.begin()+i);
            i--;
        }
    }

    // goes through all candidates and updates costs for the route that was modified
    for (int i = 0; i < candidatesCost.size(); i++) {

        // verifies if current cheapest route's previous node and next node are still "neighbors"
        // if not, we need to recalculate the cost of the route, because the previous cheapest insertion
        // is no longer valid.

        double cheapestRouteCost = get<2>(candidatesCost[i]);
        int currentPrevIndex = get<3>(candidatesCost[i]);
        int currentNextIndex = get<4>(candidatesCost[i]);

        if (!verifyNeighbor(r, currentPrevIndex, currentNextIndex))
            cheapestRouteCost = INF;

        // if candidate is in the same route as the last client inserted, we will calculate
        // the cost (delta) of inserting the candidate between the client and the previous node of the client 
        // and the client and the next node of the client and compare the costs

        if (get<1>(candidatesCost[i]) == iRoute) {
            // delta ij (k) = cik + ckj - cij
            double deltaCost, cik, ckj, cij;

            // inserting candidate between previous node of client and client
            cij = g->getManhattanDistance(clientPrevNode, client->getID());
            cik = g->getManhattanDistance(clientPrevNode, get<0>(candidatesCost[i]));
            ckj = g->getManhattanDistance(get<0>(candidatesCost[i]), client->getID());

            deltaCost = cik + ckj - cij;
            deltaCost *= CT;

            // if delta cost is less than the current cheapest cost, updates the cost
            if (deltaCost < cheapestRouteCost) {
                cheapestRouteCost = deltaCost;
                get<2>(candidatesCost[i]) = deltaCost;
                get<3>(candidatesCost[i]) = clientPrevNode;
                get<4>(candidatesCost[i]) = client->getID();
            }

            // inserting candidate between client and next node of client
            cij = g->getManhattanDistance(client->getID(), clientNextNode);
            cik = g->getManhattanDistance(client->getID(), get<0>(candidatesCost[i]));
            ckj = g->getManhattanDistance(get<0>(candidatesCost[i]), clientNextNode);

            deltaCost = cik + ckj - cij;
            deltaCost *= CT;

            // if delta cost is less than the current cheapest cost, updates the cost
            if (deltaCost < cheapestRouteCost) {
                cheapestRouteCost = deltaCost;
                //get<1>(candidatesCost[i]) = iRoute;
                get<2>(candidatesCost[i]) = deltaCost;
                get<3>(candidatesCost[i]) = client->getID();
                get<4>(candidatesCost[i]) = clientNextNode;
            }

        }
    }

    // sorts candidates by cost
    sortCandidatesByCost(g);
}

// void Solution::createDroneRoutes(Graph *g) {
//     // int launchNode, clientNode, retrieveNode;
//     // vector<Node*> launchNodesList, retrieveNodesList; 
//     // tuple<int, int, int> flight;

//     // // for each client on each truck route
//     // for (int i = 0; i < this->routes.size(); i++) { 
//     //     for (int j = 0; j < this->routes[i].getTruckRoute().size(); j++) {
//     //         Node* client = this->routes[i].getTruckRoute()[j];
//     //         Node* aux;

//     //         // if client can be reached by drone and its demand is less than the drone's capacity
//     //         if (client->getServiceBy() == DRONE_TRUCK && client->getDemand() <= QD) {
//     //             if (client->getID() != this->routes[i].getTruckRoute()[0]->getID() && client->getID() != this->routes[i].getTruckRoute()[1]->getID()) {
//     //                 clientNode = client->getID();
                    
//     //                 // creates a launching nodes list with all nodes before client (besides depot)
//     //                 // creates a retrieving nodes list with all nodes after client (besides depot)
//     //                 // chooses the closest (euclidean distance) node from the launching nodes list as the launch node
//     //                 // goes through the retrieving nodes list and chooses a node that is reachable by drone from client node (doesn't exceed flight endurance)
//     //                 // adjustes truck route mantening launching and retrieving nodes, without client node
//     //                 // removes cost of client from truck route

//     //             }

//     //         }
//     //     }
//     // }
// }

vector<pair<int,bool>> Solution::getAttendedClients() {
    return attendedClients;
}

vector<Route> Solution::getRoutes() {
    return routes;
}

void Solution::updateAttendedClients(int clientID) {
    for (int i = 0; i < attendedClients.size(); i++) {
        if (attendedClients[i].first == clientID) {
            attendedClients[i].second = true;
        }
    }
}

bool Solution::allClientsAttended(Graph *g) {
    for (int i = 0; i < g->getSize(); i++) {
        if (!attendedClients[i].second) {
            return false;
        }
    }

    return true;
}

void Solution::printRoutes() {
    for (int i = 0; i < routes.size(); i++) {
        cout << "Route " << i << ": ";
        routes[i].printRoute();
    }
    cout << endl;
}

void Solution::printCandidatesCost() {
    // print candidates list for each route
    
    for (int j = 0; j < routes.size(); j++) {
        cout << endl;
        cout << "CANDIDATES FOR ROUTE " << j << endl;
        for (int i = 0; i < candidatesCost.size(); i++) {
            if (get<1>(candidatesCost[i]) == j)
                cout << "Candidate " << get<0>(candidatesCost[i]) << " between nodes " <<  get<3>(candidatesCost[i]) << " and " << get<4>(candidatesCost[i]) <<" => delta = " << get<2>(candidatesCost[i]) << endl;
        }
    }

    cout << "------------------" << endl;
}

void Solution::plotSolution(Solution *s, string instance){
    string filename = "solutionForPlot.txt";
    ofstream output_file(filename);

    output_file << s->routes.size() << endl;
    output_file << s->getCost() << endl;

    for (int i=0; i < s->routes.size(); i++) {
        string truckRoute = "";

        for (int j=0; j < s->routes[i].getTruckRoute().size(); j++){
           truckRoute = truckRoute + "-" + to_string(s->routes[i].getTruckRoute()[j]->getID());
        }

        if(!truckRoute.empty())
            output_file << truckRoute.substr(1);
        output_file << endl;

        // string droneRoute = "";

        // for (int j=0; j < s->routes[i].getDroneRoute().size(); j++){
        //     droneRoute = droneRoute + "-" + s->routes[i].getDroneRoute()[j]->getID();
        // }

        // if(!droneRoute.empty())
        //     output_file << droneRoute.substr(1);
        output_file << endl;
    }
    
    output_file.close();
    string command = "python plotSolution.py " + instance + " " + filename;
    int aux = system(command.c_str());
    // command = "rm " + filename;
    // aux = system(command.c_str());
}

double Solution::getCost() {
    return this->cost;
}

Solution::~Solution() {
    routes.clear();
    attendedClients.clear();
}