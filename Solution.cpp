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
#define QT 25   // maximum load capacity of trucks 
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

using namespace std;

Solution::Solution(Graph *g, int QT) {
    this->QT = QT;                           // maximum load capacity of trucks
    this->totalDeliveryCost.first = 0.0;
    this->totalDeliveryCost.second = 0.0;

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
    updateSolution(g, TRUCK);            // update costs of solution
    printRoutes();
    createDroneRoutes(g);
    updateSolution(g, TRUCK_DRONE);      // update costs of solution
    printRoutes();
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
            double cost = manhattanDistance * CT * 2;
            candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost,0,0));
        }
    }

    // sorts candidates by cost
    sortCandidatesByCost(g);

    cout << endl;

    insertRandomizedFirstClients(g);

    //printRoutes();
    //printCandidatesCost();

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
            cout << "Client " << cheapestInsertion->getID() << " inserted in route " << iRoute << endl;
        
        //printCandidatesCost();
        //cout << endl;
    }

    // register final truck routes (before drones)
    for (int i = 0; i < routes.size(); i++) 
        routes[i].registerPrevTruckRoute();

    //printRoutes();
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

void Solution::insertRandomizedFirstClients(Graph *g) {
    int n = this->routes.size();
    int i = 0;
    while (i < n) {
        int randomIndex = rand() % candidatesCost.size();
        int clientID = get<0>(candidatesCost[randomIndex]);
        int routeIndex = i;
        int prevNode = get<3>(candidatesCost[randomIndex]);
        Node *client = g->getNode(clientID);
        if (includeClient(client, &routes[routeIndex], g, prevNode, routeIndex)) {
            cout << "Random client " << client->getID() << " inserted in route " << routeIndex << endl;
            i++;
        }
    }

    //printRoutes();
}

bool Solution::allClientsAttended(Graph *g) {
    for (int i = 0; i < g->getSize(); i++) {
        if (!attendedClients[i].second) {
            return false;
        }
    }

    return true;
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
    r->updateDeliveryCost(g, CT, CD, CB, TRUCK);
    
    // updates cheapest insertion candidates list
    updateCandidatesList(client, r, g, iRoute);
    return true;
}

void Solution::updateAttendedClients(int clientID) {
    for (int i = 0; i < attendedClients.size(); i++) {
        if (attendedClients[i].first == clientID) {
            attendedClients[i].second = true;
        }
    }
}

void Solution::updateSolution(Graph *g, int typeOfSolution) {
    double f1 = 0.0, f2 = 0.0, f3 = 0.0;

    for (int i = 0; i < routes.size(); i++) {
        routes[i].updateEnergyConsumption(g, typeOfSolution, QT);
        routes[i].updateDeliveryTime(g, typeOfSolution, ST, SD);

        f1 += routes[i].getEnergyConsumption(typeOfSolution);
        f2 += routes[i].getDeliveryCost(typeOfSolution);
        f3 += routes[i].getDeliveryTime(typeOfSolution);
    }

    if (typeOfSolution == TRUCK_DRONE) {
        this->totalDeliveryCost.second = f1;
        this->totalDeliveryCost.second = f2;
        this->totalDeliveryCost.second = f3;
    } else if (typeOfSolution == TRUCK) {
        this->totalDeliveryCost.first = f1;
        this->totalDeliveryCost.first = f2;
        this->totalDeliveryCost.first = f3;
    }
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

bool isInSearchRange(vector<int> searchRange, int clientID) {
    for (int i = 0; i < searchRange.size(); i++) {
        if (searchRange[i] == clientID) {
            cout << "Client " << clientID << " is in search range" << endl;
            return true;
        }
    }
    cout << "Client " << clientID << " is not in search range" << endl;
    return false;
}

void Solution::createDroneRoutes(Graph *g) {
    int launchNode, clientNode, retrieveNode;
    vector<int> launchNodesList, retrieveNodesList; 
    tuple<int, int, int> flight(0,0,0);

    // for each client on each truck route
    for (int i = 0; i < this->routes.size(); i++) { 
        vector<Node*> tRoute = this->routes[i].getTruckRoute();
        vector<tuple<int, int, int, double, bool>> bestFlight;  // <clientID, launchNode, retrieveNode, gain, is possible>
        vector<int> searchRange;

        // creates a list with all clients on the route (every node besides except depot)
        for (int k = 1; k < tRoute.size()-1; k++) {
            searchRange.push_back(tRoute[k]->getID());
        }

        for (int j = 0; j < tRoute.size(); j++) {

            if (isInSearchRange(searchRange, tRoute[j]->getID())) {
                Node* client = this->routes[i].getTruckRoute()[j];

                // if client can be reached by drone and its demand is less than the drone's capacity
                if (client->getServiceBy() == DRONE_TRUCK && client->getDemand() <= QD) {
                    // verifies if node isn't depot or first/last node on route
                    if ((client->getID() != tRoute[0]->getID()) && (client->getID() != tRoute[1]->getID()) && (client->getID() != tRoute[tRoute.size()-2]->getID())) {
                        cout << endl << endl << "ROUTE: " << i << " | CLIENT: " << client->getID();
                        clientNode = client->getID();
                        get<1>(flight) = clientNode;

                        // creates a launching nodes list with all nodes before client
                        int k;
                        launchNodesList.clear();
                        for (k = 0; k < searchRange.size(); k++) {
                            if (searchRange[k] == clientNode)
                                break;

                            launchNodesList.push_back(searchRange[k]);
                        }
                        // sort launching nodes list by euclidean distance to client
                        sortListByEuclideanDistance(g, &launchNodesList, clientNode);


                        // creates a retrieving nodes list with all nodes after client
                        retrieveNodesList.clear();
                        for (k = k+1; k < searchRange.size(); k++) {
                            retrieveNodesList.push_back(searchRange[k]);
                        }

                        // goes through lauching and retrieving nodes list, choosing flights that doesn't exceed flight endurance
                        //get<4>(flight) = false;
                        get<1>(flight) = clientNode;
                        tuple<int,int,int> bestClientFlight;
                        double biggestGain = -INF;

                        for (int n = 0; n < launchNodesList.size(); n++) {
                            get<0>(flight) = launchNodesList[n];
                            for (k = 0; k < retrieveNodesList.size(); k++) {
                                get<2>(flight) = retrieveNodesList[k];

                                if (isReachableByDrone(g, flight, i)) {
                                    double droneFlightCost = g->getEuclideanDistance(get<0>(flight), get<1>(flight)) + g->getEuclideanDistance(get<1>(flight), get<2>(flight));
                                    double truckRouteCost;
                                    for (int l = 0; l < tRoute.size(); l++) {
                                        if (tRoute[l]->getID() == get<1>(flight)) {
                                            truckRouteCost = g->getManhattanDistance(tRoute[l-1]->getID(), get<1>(flight)) + g->getManhattanDistance(get<1>(flight), tRoute[l+1]->getID());
                                            break;
                                        }
                                    }

                                    double gain = truckRouteCost - droneFlightCost;
                                    cout << "ganho: " << gain << endl;

                                    if (gain > biggestGain) {
                                        get<0>(bestClientFlight) = get<0>(flight);
                                        get<1>(bestClientFlight) = get<1>(flight);
                                        get<2>(bestClientFlight) = get<2>(flight);
                                        biggestGain = gain;
                                    }
                                }
                            }
                        }
                        
                        // if the best flight is better than the truck route, adds it to the best flight list
                        if (biggestGain > 0) {
                            cout << endl << "best flight for client " << client->getID() << ": (" << get<0>(bestClientFlight) << "," << get<1>(bestClientFlight) << "," << get<2>(bestClientFlight) << ")" << endl;
                            bestFlight.push_back(make_tuple(get<1>(bestClientFlight), get<0>(bestClientFlight), get<2>(bestClientFlight), biggestGain, false));
                        }

                    } else {
                        cout << "Node " << client->getID() << " can't be attended by drone (is depot or first/last node after/before depot)." << endl;
                    }
                }
            }
        }

        // sorts best flights by gain
        sortListByGain(&bestFlight);
        
        // adds best flights to route
        for (int j = 0; j < bestFlight.size(); j++) {
            //tuple<int,int,int> choosenFlight(get<1>(bestFlight[j]), get<0>(bestFlight[j]), get<2>(bestFlight[j]));
            //get<4>(bestFlight[j]) = true;

            get<0>(flight) = get<1>(bestFlight[j]);
            get<1>(flight) = get<0>(bestFlight[j]);
            get<2>(flight) = get<2>(bestFlight[j]);
            this->routes[i].insertDroneFlight(flight);
        }

        // corrects truck route (removes clients that were served by drone)
        this->routes[i].removeClientsServedByDrone(g, CT, CD, CB);
        bestFlight.clear();
        searchRange.clear();
    }
}

void Solution::sortListByGain(vector<tuple<int, int, int, double, bool>> *list) {
    int i, j;
    tuple<int, int, int, double, bool> aux;

    for (i = 0; i < list->size(); i++) {
        for (j = i+1; j < list->size(); j++) {
            if (get<3>(list->at(j)) > get<3>(list->at(i))) {
                aux = list->at(i);
                list->at(i) = list->at(j);
                list->at(j) = aux;
            }
        }
    }
}

void Solution::sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode) {
    int i, j, aux;
    double cij, cik, ckj;

    for (i = 0; i < nodeIdList->size(); i++) {
        for (j = i+1; j < nodeIdList->size(); j++) {
            cij = g->getEuclideanDistance(clientNode, nodeIdList->at(i));
            cik = g->getEuclideanDistance(clientNode, nodeIdList->at(j));
            if (cik < cij) {
                aux = nodeIdList->at(i);
                nodeIdList->at(i) = nodeIdList->at(j);
                nodeIdList->at(j) = aux;
            }
        }
    }
}

bool Solution::isReachableByDrone(Graph *g, tuple<int, int, int> flight, int routeIndex) {

    cout << endl << endl << "verifying flight: [" << get<0>(flight) << ", " << get<1>(flight) << ", " << get<2>(flight) << "]" << endl;

    int launchNode = get<0>(flight);
    int clientNode = get<1>(flight);
    int retrieveNode = get<2>(flight);

    double loading_rate = g->getNode(clientNode)->getDemand() / QD;
    double dynamic_scaling_factor = 1 - 0.2 * loading_rate;

    // calculates expected maximum flight endurance
    double loadedFlight = (g->getEuclideanDistance(launchNode, clientNode) / (g->getEuclideanDistance(launchNode, clientNode) + g->getEuclideanDistance(clientNode, retrieveNode))) * dynamic_scaling_factor;
    double emptyFlight = (g->getEuclideanDistance(clientNode, retrieveNode) / (g->getEuclideanDistance(launchNode, clientNode) + g->getEuclideanDistance(clientNode, retrieveNode)));

    double expct_max_flight_endurance = (loadedFlight + emptyFlight)*E;
    
    // if expected maximum flight endurance is greater or equal to the time needed to travel 
    // from launch node to client node and from client node to retrieve node, returns true

    double t_ij = g->getEuclideanDistance(launchNode, clientNode) / SD;
    double t_jk = g->getEuclideanDistance(clientNode, retrieveNode) / SD;

    // cout << "expct_max_flight_endurance: " << expct_max_flight_endurance << endl;
    // cout << "time spent: " << t_ij + t_jk << endl;

    if ((t_ij + t_jk) <= expct_max_flight_endurance) {

        // verifies if truck will be able to reach client node before or with drone
        double droneTime = t_ij + t_jk;
        cout << "drone time: " << droneTime << endl;

        int nodeID = 0, index = 0;
        while (nodeID != launchNode) {
            index++;
            nodeID = this->routes[routeIndex].getNode(index)->getID();
        }

        // launch node is in index position in route
        // verifies nodes before retrieve node, besides client node, and calculates time spent
        double truckTime = 0;
        int nextNodeID = 0;

        while (nextNodeID != retrieveNode) {
            nodeID = this->routes[routeIndex].getNode(index)->getID();
            index++;
            nextNodeID = this->routes[routeIndex].getNode(index)->getID();
            if (nextNodeID != clientNode) {
                truckTime += g->getManhattanDistance(nodeID, nextNodeID) / ST;  
                cout << "truck time between " << nodeID << " and " << nextNodeID << ": " << g->getManhattanDistance(nodeID, nextNodeID) / ST << endl;
            }
            else {
                cout << nextNodeID << " is client! skipping..." << endl;
                index++;
                nextNodeID = this->routes[routeIndex].getNode(index)->getID();
                truckTime += g->getManhattanDistance(nodeID, nextNodeID) / ST;
                cout << "truck time between " << nodeID << " and " << nextNodeID << ": " << g->getManhattanDistance(nodeID, nextNodeID) / ST << endl;
            }
        }

        cout << "total truck time: " << truckTime << endl;

        if (truckTime <= droneTime) {
            cout << "flight is possible" << endl;
            return true;
        }
    }

    cout << "flight is not possible" << endl;
    return false;
}

void Solution::updateSearchRange(vector<int>*searchRange, int rNode) {
    cout << endl << "updating search range" << endl;

    int firstNode = searchRange->front();

    while (firstNode != rNode) {
        searchRange->erase(searchRange->begin());
        firstNode = searchRange->front();
    }

    cout << "search range updated: ";

    for (int i = 0; i < searchRange->size(); i++) {
        cout << searchRange->at(i) << " ";
    }

    cout << endl;
}

void Solution::printRoutes() {
    cout << endl;
    for (int i = 0; i < routes.size(); i++) {
        cout << "Route " << i << ": ";
        routes[i].printRoute();
    }
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
    
    // plot truck solution
    string filename0 = "truckSolutionForPlot.txt";
    ofstream output_file0(filename0);

    output_file0 << s->routes.size() << endl;
    output_file0 << s->getTotalEnergyConsumption().first << endl;
    output_file0 << s->getTotalDeliveryTime().first << endl;
    output_file0 << s->getTotalDeliveryCost().first << endl;

    for (int i=0; i < s->routes.size(); i++) {
        string truckRoute = "";

        for (int j=0; j < s->routes[i].getPrevTruckRoute().size(); j++){
           truckRoute = truckRoute + "-" + to_string(s->routes[i].getPrevTruckRoute()[j]);
        }

        if(!truckRoute.empty())
            output_file0 << truckRoute.substr(1);
        output_file0 << endl;
        output_file0 << endl;
    }

    // plot truck+drone solution
    string filename = "solutionForPlot.txt";
    ofstream output_file(filename);

    output_file << s->routes.size() << endl;
    output_file << s->getTotalEnergyConsumption().second << endl;
    output_file << s->getTotalDeliveryTime().second << endl;
    output_file << s->getTotalDeliveryCost().second << endl;

    for (int i=0; i < s->routes.size(); i++) {
        string truckRoute = "";

        for (int j=0; j < s->routes[i].getTruckRoute().size(); j++){
           truckRoute = truckRoute + "-" + to_string(s->routes[i].getTruckRoute()[j]->getID());
        }

        if(!truckRoute.empty())
            output_file << truckRoute.substr(1);
        output_file << endl;

        string droneRoute = " ";

        for (int j=0; j < s->routes[i].getDroneRoute().size(); j++){
            droneRoute = droneRoute + "/" + to_string(get<0>(s->routes[i].getDroneRoute()[j]))
                                    + "-" + to_string(get<1>(s->routes[i].getDroneRoute()[j]))
                                    + "-" + to_string(get<2>(s->routes[i].getDroneRoute()[j]));
        }

        if(!droneRoute.empty())
            output_file << droneRoute.substr(1);
        output_file << endl;
    }

    output_file0.close();
    output_file.close();
    string command = "cd ..";
    int aux = system(command.c_str());
    command = "python plotSolution.py " + instance + " " + filename0 + " " + filename;
    aux = system(command.c_str());
    // command = "rm " + filename;
    // aux = system(command.c_str());
}

vector<pair<int,bool>> Solution::getAttendedClients() {
    return attendedClients;
}

vector<Route> Solution::getRoutes() {
    return routes;
}

pair<double,double> Solution::getTotalDeliveryCost() {
    return this->totalDeliveryCost;
}

pair<double,double> Solution::getTotalEnergyConsumption() {
    return this->totalEnergyConsumption;
}

pair<double,double> Solution::getTotalDeliveryTime() {
    return this->totalDeliveryTime;
}

Solution::~Solution() {
    routes.clear();
    attendedClients.clear();
}