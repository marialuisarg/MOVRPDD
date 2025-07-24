#include "Solution.hpp" 
#include "Utils.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <cmath>
#include <fstream>
#include <tuple>
#include <random>
#include <algorithm>

Solution::Solution(Graph *g, int QT, RandomGenerator *randGen) {
    this->QT = QT;                           // maximum load capacity of trucks
    this->totalDeliveryCost = 0.0;
    this->totalEnergyConsumption = 0.0;
    this->totalDeliveryTime = 0.0;
    this->drone = false;
    this->dominatedBy = 0;
    this->randGen = randGen;
    this->localSearch = false;

    setNumRoutes(0);
}

void Solution::updateSolution(Graph *g) {
    double f1 = 0.0, f2 = 0.0, f3 = 0.0;

    for (int i = 0; i < getNumRoutes(); i++) {
        routes[i]->updateEnergyConsumption(g, QT);
        routes[i]->updateDeliveryTime(g, ST, SD);

        f1 += routes[i]->getEnergyConsumption();
        f2 += routes[i]->getDeliveryCost();
        if (routes[i]->getDeliveryTime() > f3)
            f3 = routes[i]->getDeliveryTime();
    }

    setTotalEnergyConsumption(f1);
    setTotalDeliveryCost(f2);
    setTotalDeliveryTime(f3);
}

void Solution::printRoutes() {
    cout << endl;
    for (int i = 0; i < getNumRoutes(); i++) {
        cout << "Route " << i << ": ";
        routes[i]->printRoute();
    }
}

void Solution::plotSolution(string instance, int i, string filename) {
    // plot solution
    string instanceName = instance;
    
    // if (droneIsUsed())
    //     filename = "./solutions/droneSolution" + to_string(i) + "_" + instance.erase(0,12);
    // else
    //     filename = "./solutions/truckSolution" + to_string(i) + "_" + instance.erase(0,12);
    
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

        for (int j=0; j < this->routes[i]->getTruckRoute().size(); j++){
            truckRoute = truckRoute + "-" + to_string(this->routes[i]->getTruckRoute()[j]->getID());
        }

        if(!truckRoute.empty())
            output_file << truckRoute.substr(1);
        output_file << endl;

        string droneRoute = " ";

        for (int j=0; j < this->routes[i]->getDroneRoute().size(); j++){
            droneRoute = droneRoute + "/" + to_string(get<0>(this->routes[i]->getDroneRoute()[j]))
                                    + "-" + to_string(get<1>(this->routes[i]->getDroneRoute()[j]))
                                    + "-" + to_string(get<2>(this->routes[i]->getDroneRoute()[j]));
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

void Solution::saveRouteToPlot(std::ofstream &output_file) {
    int nRoutes = getNumRoutes();

    //std::cout << "Saving route to plot" << std::endl;

    for (int i=0; i < nRoutes; i++) {
        string truckRoute = "";

        for (int j=0; j < this->routes[i]->getTruckRoute().size(); j++){
            truckRoute = truckRoute + "-" + to_string(this->routes[i]->getTruckRoute()[j]->getID());
        }

        if(!truckRoute.empty())
            output_file << truckRoute.substr(1);
        output_file << std::endl;

        string droneRoute = " ";

        for (int j=0; j < this->routes[i]->getDroneRoute().size(); j++){
            droneRoute = droneRoute + "/" + to_string(get<0>(this->routes[i]->getDroneRoute()[j]))
                                    + "-" + to_string(get<1>(this->routes[i]->getDroneRoute()[j]))
                                    + "-" + to_string(get<2>(this->routes[i]->getDroneRoute()[j]));
        }

        if(!droneRoute.empty())
            output_file << droneRoute.substr(1);
        output_file << endl;
    }
}

void Solution::sortCandidatesByCost(Graph* g) {
    // int n = this->getCandidatesCost().size();
    // tuple<int, int, double, int, int> temp;
    
    // int i, j;
    // for (i = 0; i < n - 1; i++) {
    //     for (j = 0; j < n - i - 1; j++) {
    //         if (get<2>(this->getCandidateCost(j)) > get<2>(this->getCandidateCost(j+1))) {
    //             temp = this->getCandidateCost(j);
    //             this->setCandidateCost(j, this->getCandidateCost(j+1));
    //             this->setCandidateCost(j+1, temp);
    //         }
    //     }
    // }

    auto& candidates = this->getCandidatesCost();

    std::sort(candidates.begin(), candidates.end(),
        [](const auto& a, const auto& b) {
            return std::get<2>(a) < std::get<2>(b);
        });
}

void Solution::includeRoute(Route* route) {
    this->routes.push_back(route);
    this->setNumRoutes(this->getNumRoutes() + 1);
}

bool Solution::includeClient(Node* client, Graph* g, int prevNode, int routeIndex) {
    Route* r = this->routes[routeIndex];

    // verifies if client is already attended
    if (this->getAttendedClient(client->getID()).second) {
        return false;
    }
    
    // inserts client in route
    if (!r->insertClientPrev(client, prevNode)) {
        for (int i = 0; i < this->getCandidatesCost().size(); i++) {
            if (get<0>(this->getCandidateCost(i)) == client->getID() && get<1>(this->getCandidateCost(i)) == routeIndex) {
                this->eraseCandidateCostAt(i);
                break;
            }
        }

        return false;
    }

    // updates attended clients
    this->updateAttendedClients(client->getID());
    //r->updateDeliveryCost(g, CT, CD, CB);
    
    // updates cheapest insertion candidates list
    this->updateCandidatesList(client, g, routeIndex);
    return true;
}

bool verifyNeighbor(Route *r, int currentPrevIndex, int currentNextIndex) {
    for (int j = 0; j < r->getTruckRoute().size()-1; j++)
        if (r->getTruckRoute()[j]->getID() == currentPrevIndex)
            if (r->getTruckRoute()[j+1]->getID() != currentNextIndex)
                return false;
    return true;
}

void Solution::updateAttendedClients(int clientID) {
    for (int i = 0; i < this->getAttendedClients().size(); i++) {
        if (this->getAttendedClient(i).first == clientID) {
            this->setAttendedClient(i, true);
        }
    }
}

void Solution::updateCandidatesList(Node *client, Graph *g, int iRoute) {
   
   int clientPrevNode = 0, clientNextNode = 0;

   // remove client from candidates list
    for (int i = 0; i < this->getCandidatesCost().size(); i++) {
        if (get<0>(this->getCandidateCost(i)) == client->getID()) {
            if (get<1>(this->getCandidateCost(i)) == iRoute) {
                clientPrevNode = get<3>(this->getCandidateCost(i));
                clientNextNode = get<4>(this->getCandidateCost(i));
            }

            this->eraseCandidateCostAt(i);
            i--;
        }
    }

    // goes through all candidates and updates costs for the route that was modified
    for (int i = 0; i < this->getCandidatesCost().size(); i++) {

        // verifies if current cheapest route's previous node and next node are still "neighbors"
        // if not, we need to recalculate the cost of the route, because the previous cheapest insertion
        // is no longer valid.

        double cheapestRouteCost = get<2>(this->getCandidateCost(i));
        int currentPrevIndex = get<3>(this->getCandidateCost(i));
        int currentNextIndex = get<4>(this->getCandidateCost(i));

        if (!verifyNeighbor(this->getRoute(iRoute), currentPrevIndex, currentNextIndex))
            cheapestRouteCost = INF;

        // if candidate is in the same route as the last client inserted, we will calculate
        // the cost (delta) of inserting the candidate between the client and the previous node of the client 
        // and the client and the next node of the client and compare the costs

        if (get<1>(this->getCandidateCost(i)) == iRoute) {
            // delta ij (k) = cik + ckj - cij
            double deltaCost, cik, ckj, cij;

            // inserting candidate between previous node of client and client
            cij = g->getManhattanDistance(clientPrevNode, client->getID());
            cik = g->getManhattanDistance(clientPrevNode, get<0>(this->getCandidateCost(i)));
            ckj = g->getManhattanDistance(get<0>(this->getCandidateCost(i)), client->getID());

            deltaCost = cik + ckj - cij;
            deltaCost *= CT;

            // if delta cost is less than the current cheapest cost, updates the cost
            if (deltaCost < cheapestRouteCost) {
                cheapestRouteCost = deltaCost;
                tuple<int, int, double, int, int> aux = this->getCandidateCost(i);
                get<2>(aux) = deltaCost;
                get<3>(aux) = clientPrevNode;
                get<4>(aux) = client->getID();
                this->setCandidateCost(i, aux);
            }

            // inserting candidate between client and next node of client
            cij = g->getManhattanDistance(client->getID(), clientNextNode);
            cik = g->getManhattanDistance(client->getID(), get<0>(this->getCandidateCost(i)));
            ckj = g->getManhattanDistance(get<0>(this->getCandidateCost(i)), clientNextNode);

            deltaCost = cik + ckj - cij;
            deltaCost *= CT;

            // if delta cost is less than the current cheapest cost, updates the cost
            if (deltaCost < cheapestRouteCost) {
                cheapestRouteCost = deltaCost;
                //get<1>(this->getCandidateCost(i)) = iRoute;
                tuple<int, int, double, int, int> aux = this->getCandidateCost(i);
                get<2>(aux) = deltaCost;
                get<3>(aux) = client->getID();
                get<4>(aux) = clientNextNode;
                this->setCandidateCost(i, aux);
            }

        }
    }

    // sorts candidates by cost
    sortCandidatesByCost(g);
}

void Solution::eraseCandidateCostAt(int i) { 
    //cout << "erasing candidate " << get<0>(this->getCandidateCost(i)) << " from route " << get<1>(this->getCandidateCost(i)) << endl << endl;
    this->candidatesCost.erase(this->candidatesCost.begin() + i); 
};

bool Solution::dominates(Solution *s) {
    if (s == nullptr)
        return true;
    
    double c_f1 = this->getTotalEnergyConsumption();
    double c_f2 = this->getTotalDeliveryCost();
    double c_f3 = this->getTotalDeliveryTime();

    double b_f1 = s->getTotalEnergyConsumption();
    double b_f2 = s->getTotalDeliveryCost();
    double b_f3 = s->getTotalDeliveryTime();

    // checks if current solution dominates the other solution
    if (c_f1 < b_f1 && c_f2 <= b_f2 && c_f3 <= b_f3)
        return true;
    else if (c_f1 <= b_f1 && c_f2 < b_f2 && c_f3 <= b_f3)
        return true;
    else if (c_f1 <= b_f1 && c_f2 <= b_f2 && c_f3 < b_f3)
        return true;

    return false;
}

vector<Route*> Solution::getRoutes() {
    return routes;
}

bool Solution::allClientsAttended(Graph *g) {
    for (int i = 0; i <= this->getNumClients(); i++) {
        if (!this->getAttendedClient(i).second) {
            return false;
        }
    }
    return true;
}

unsigned int Solution::random(int min, int max) {
    if (max < 0) max = 0;
    return randGen->getInt(min, max);
}

vector<int> Solution::encode() {
    
    // first marker = 0
    // other markers = numClients + 1, numClients + 2, ...

    int  marker = this->getNumClients() + 1;
    int  numRoutes = getNumRoutes();
    vector<int> encodedSol;  // encoded solution size = numClients + markers for each route di
    
    for (int i = 0; i < numRoutes; i++) {
        Route *route = getRoute(i);
        
        // for (int j = 1; j < route->getPrevTruckRoute().size()-1; j++) {
        //     encodedSol.push_back(route->getPrevTruckRoute()[j]);
        // }

        if (i == numRoutes-1) break;

        if (i != 0) {
            encodedSol.push_back(marker);
            //cout << "Inserting marker " << marker << " at position " << encodedSol.size()-1 << endl;
            marker++;
        } else {
            encodedSol.push_back(0);
        }
    }

    //printEncodedSolution(encodedSol);

    return encodedSol;
};

double Solution::getObjective(int obj) {
    if (obj == 0)
        return getTotalEnergyConsumption();

    if (obj == 1)
        return getTotalDeliveryCost();
    
    return getTotalDeliveryTime();
}

void Solution::printEncodedSolution(vector<int> sol) {
    cout << "-------------------" << endl;
    cout << "ENCODED SOLUTION: " << endl;
    for (auto it = sol.begin(); it != sol.end(); it++)
        cout << *it << " ";

    cout << " | Number of nodes: " << sol.size() << endl; 
    cout << endl << "-------------------" << endl;
};

void Solution::printSolution() {
    cout << "-------------------" << endl;
    cout << "SOLUTION: " << endl;
    cout << "Number of routes: " << getNumRoutes() << endl;
    cout << "Total energy consumption: " << getTotalEnergyConsumption() << endl;
    cout << "Total delivery cost: " << getTotalDeliveryCost() << endl;
    cout << "Total delivery time: " << getTotalDeliveryTime() << endl;
    cout << "Number of clients: " << getNumClients() << endl;
    cout << "Attended clients: " << endl;
    int count = 0;
    for (int i = 0; i <= getNumClients(); i++) {
        if (getAttendedClient(i).second) {
            cout << getAttendedClient(i).first << " ";
            count++;
        }
    }
    cout << endl;
    cout << "Number of attended clients: " << count << endl;
    cout << "-------------------" << endl;
};

Solution::~Solution() {
    routes.clear();
};