#include "GreedyConstructor.h" 
#include "Solution.h"
#include <iostream>

#include <vector>
#include <string>
#include <string.h>
#include <cmath>
#include <fstream>
#include <tuple>
#include <utility>

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

using namespace std;

Solution * greedyConstructor(Graph *g, int QT) {
    int numRoutes;
    bool droneRouteCreated;

    vector<pair<int, bool>> attendedClients;

    Solution *sol = new Solution(g, QT);     // creates solution

    for (int i = 0; i < g->getSize(); i++) {
        attendedClients.push_back({g->getNode(i)->getID(), false});  
    }

    sol->setAttendedClients(attendedClients);

    // depot visited 
    sol->setAttendedClient(0, true);

    // creates routes
    for (int i = 0; i < sol->getNumRoutes(); i++) {
        Route r(QT, QD, g->getNode(0));
        sol->createRoute(r);
    }

    createTruckRoutes(g, sol, &numRoutes, &droneRouteCreated);
    createDroneRoutes(g, sol);
    return sol;
}

void createTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated) {

    // creates list of candidates
    vector<Node*> candidates;
    vector<tuple<int, int, double, int, int>> candidatesCost;

    // inserts unattended clients into the candidate list
    for (int i = 0; i < g->getSize(); i++) {
        if (!sol->getAttendedClient(i).second) {
            candidates.push_back(g->getNode(i));
        }
    }

    // calculates cost of each candidate
    for (int r = 0; r < sol->getNumRoutes(); r++) {
        for (int i = 0; i < candidates.size(); i++) {
            double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
            double cost = manhattanDistance * CT * 2;
            candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost,0,0));
        }
    }

    sol->setCandidatesCost(candidatesCost);
    printCandidatesCost(sol);

    // sorts candidates by cost
    sol->sortCandidatesByCost(g);

    //cout << endl;
    insertRandomizedFirstClients(g, sol, numRoutes, droneRouteCreated);

    //printRoutes();
    //printCandidatesCost(sol);

    // while there are unattended clients
    while (!sol->allClientsAttended(g)) {

        // gets cheapest insertion
        Node *cheapestInsertion = g->getNode(get<0>(sol->getCandidateCost(0)));

        // gets route index
        int iRoute = get<1>(sol->getCandidateCost(0));

        // gets previous node index in route
        long int prevNode = get<3>(sol->getCandidateCost(0));

        // inserts client in route ("if" logic here is just for debugging, but function must be called)
        bool clientIn = sol->includeClient(cheapestInsertion, g, prevNode, iRoute);
            //cout << "Client " << cheapestInsertion->getID() << " inserted in route " << iRoute << endl;
        
        //printCandidatesCost();
        //cout << endl;
    }
    // register final truck routes (before drones)
    for (int i = 0; i < sol->getNumRoutes(); i++) 
        sol->getRoute(i)->registerPrevTruckRoute();

    //printRoutes();
    sol->updateSolution(g);
}

// void sortCandidatesByCost(Graph *g, Solution *sol) {
//     int n = sol->getCandidatesCost().size();
//     tuple<int, int, double, int, int> temp;
    
//     int i, j;
//     for (i = 0; i < n - 1; i++) {
//         for (j = 0; j < n - i - 1; j++) {
//             if (get<2>(sol->getCandidateCost(j)) > get<2>(sol->getCandidateCost(j+1))) {
//                 temp = sol->getCandidateCost(j);
//                 sol->setCandidateCost(j, sol->getCandidateCost(j+1));
//                 sol->setCandidateCost(j+1, temp);
//             }
//         }
//     }
// }

void insertRandomizedFirstClients(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated) {
    int n = sol->getNumRoutes();
    int i = 0;
    while (i < n) {
        int randomIndex = rand() % sol->getCandidatesCost().size();
        int clientID = get<0>(sol->getCandidateCost(randomIndex));
        int routeIndex = i;
        int prevNode = get<3>(sol->getCandidateCost(randomIndex));
        Node *client = g->getNode(clientID);
        if (sol->includeClient(client, g, prevNode, routeIndex)) {
            //cout << "Random client " << client->getID() << " inserted in route " << routeIndex << endl;
            i++;
        }
    }

    //printRoutes();
}

// bool includeClient(Node *client, Solution *sol, Route *r, Graph *g, int prevNodeIndex, int iRoute) {

//     // inserts client in route
//     if (!r->insertClient(client, prevNodeIndex)) {
//         for (int i = 0; i < sol->getCandidatesCost().size(); i++) {
//             if (get<0>(sol->getCandidateCost(i)) == client->getID() && get<1>(sol->getCandidateCost(i)) == iRoute) {
//                 sol->eraseCandidateCostAt(i);
//                 break;
//             }
//         }

//         return false;
//     }

//     // updates attended clients
//     updateAttendedClients(sol, client->getID());
//     r->updateDeliveryCost(g, CT, CD, CB);
    
//     // updates cheapest insertion candidates list
//     updateCandidatesList(client, sol, r, g, iRoute);
//     return true;
// }

// void updateAttendedClients(Solution* sol, int clientID) {
//     for (int i = 0; i < sol->getAttendedClients().size(); i++) {
//         if (sol->getAttendedClient(i).first == clientID) {
//             sol->setAttendedClient(i, true);
//         }
//     }
// }

// bool verifyNeighbor(Route *r, int currentPrevIndex, int currentNextIndex) {
//     for (int j = 0; j < r->getTruckRoute().size()-1; j++)
//         if (r->getTruckRoute()[j]->getID() == currentPrevIndex)
//             if (r->getTruckRoute()[j+1]->getID() != currentNextIndex)
//                 return false;
//     return true;
// }

// void updateCandidatesList(Node *client, Solution *sol, Route *r, Graph *g, int iRoute) {
   
//    int clientPrevNode = 0, clientNextNode = 0;

//    // remove client from candidates list
//     for (int i = 0; i < sol->getCandidatesCost().size(); i++) {
//         if (get<0>(sol->getCandidateCost(i)) == client->getID()) {
//             if (get<1>(sol->getCandidateCost(i)) == iRoute) {
//                 clientPrevNode = get<3>(sol->getCandidateCost(i));
//                 clientNextNode = get<4>(sol->getCandidateCost(i));
//             }

//             sol->eraseCandidateCostAt(i);
//             i--;
//         }
//     }

//     // goes through all candidates and updates costs for the route that was modified
//     for (int i = 0; i < sol->getCandidatesCost().size(); i++) {

//         // verifies if current cheapest route's previous node and next node are still "neighbors"
//         // if not, we need to recalculate the cost of the route, because the previous cheapest insertion
//         // is no longer valid.

//         double cheapestRouteCost = get<2>(sol->getCandidateCost(i));
//         int currentPrevIndex = get<3>(sol->getCandidateCost(i));
//         int currentNextIndex = get<4>(sol->getCandidateCost(i));

//         if (!verifyNeighbor(r, currentPrevIndex, currentNextIndex))
//             cheapestRouteCost = INF;

//         // if candidate is in the same route as the last client inserted, we will calculate
//         // the cost (delta) of inserting the candidate between the client and the previous node of the client 
//         // and the client and the next node of the client and compare the costs

//         if (get<1>(sol->getCandidateCost(i)) == iRoute) {
//             // delta ij (k) = cik + ckj - cij
//             double deltaCost, cik, ckj, cij;

//             // inserting candidate between previous node of client and client
//             cij = g->getManhattanDistance(clientPrevNode, client->getID());
//             cik = g->getManhattanDistance(clientPrevNode, get<0>(sol->getCandidateCost(i)));
//             ckj = g->getManhattanDistance(get<0>(sol->getCandidateCost(i)), client->getID());

//             deltaCost = cik + ckj - cij;
//             deltaCost *= CT;

//             // if delta cost is less than the current cheapest cost, updates the cost
//             if (deltaCost < cheapestRouteCost) {
//                 cheapestRouteCost = deltaCost;
//                 tuple<int, int, double, int, int> aux = sol->getCandidateCost(i);
//                 get<2>(aux) = deltaCost;
//                 get<3>(aux) = clientPrevNode;
//                 get<4>(aux) = client->getID();
//                 sol->setCandidateCost(i, aux);
//             }

//             // inserting candidate between client and next node of client
//             cij = g->getManhattanDistance(client->getID(), clientNextNode);
//             cik = g->getManhattanDistance(client->getID(), get<0>(sol->getCandidateCost(i)));
//             ckj = g->getManhattanDistance(get<0>(sol->getCandidateCost(i)), clientNextNode);

//             deltaCost = cik + ckj - cij;
//             deltaCost *= CT;

//             // if delta cost is less than the current cheapest cost, updates the cost
//             if (deltaCost < cheapestRouteCost) {
//                 cheapestRouteCost = deltaCost;
//                 //get<1>(sol->getCandidateCost(i)) = iRoute;
//                 tuple<int, int, double, int, int> aux = sol->getCandidateCost(i);
//                 get<2>(aux) = deltaCost;
//                 get<3>(aux) = client->getID();
//                 get<4>(aux) = clientNextNode;
//                 sol->setCandidateCost(i, aux);
//             }

//         }
//     }

//     // sorts candidates by cost
//     sortCandidatesByCost(g, sol);
// }

bool isInSearchRange(vector<int> searchRange, int clientID) {
    for (int i = 0; i < searchRange.size(); i++) {
        if (searchRange[i] == clientID) {
            //cout << "Client " << clientID << " is in search range" << endl;
            return true;
        }
    }
    //cout << "Client " << clientID << " is not in search range" << endl;
    return false;
}

void createDroneRoutes(Graph *g, Solution *sol) {
    int launchNode, clientNode, retrieveNode;
    vector<int> launchNodesList, retrieveNodesList; 
    tuple<int, int, int> flight(0,0,0);

    // for each client on each truck route
    for (int i = 0; i < sol->getNumRoutes(); i++) { 
        vector<Node*> tRoute = sol->getRoute(i)->getTruckRoute();
        vector<tuple<int, int, int, double, bool>> bestFlight;  // <clientID, launchNode, retrieveNode, gain, is possible>
        vector<int> searchRange;

        // creates a list with all clients on the route (every node besides depot)
        for (int k = 1; k < tRoute.size()-1; k++) {
            searchRange.push_back(tRoute[k]->getID());
        }

        for (int j = 0; j < tRoute.size(); j++) {

            if (isInSearchRange(searchRange, tRoute[j]->getID())) {
                Node* client = sol->getRoute(i)->getTruckRoute()[j];
                //cout << client->getServiceBy() << " | " << client->getDemand() << " | " << QD << endl;
                // if client can be reached by drone and its demand is less than the drone's capacity
                if (client->getServiceBy() == DRONE_TRUCK && client->getDemand() <= QD) {
                    // verifies if node isn't depot or first/last node on route
                    if ((client->getID() != tRoute[0]->getID()) && (client->getID() != tRoute[1]->getID()) && (client->getID() != tRoute[tRoute.size()-2]->getID())) {
                        //cout << endl << endl << "ROUTE: " << i << " | CLIENT: " << client->getID();
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

                                if (isReachableByDrone(g, sol, flight, i)) {
                                    double droneFlightCost = g->getEuclideanDistance(get<0>(flight), get<1>(flight)) + g->getEuclideanDistance(get<1>(flight), get<2>(flight));
                                    double truckRouteCost;
                                    for (int l = 0; l < tRoute.size(); l++) {
                                        if (tRoute[l]->getID() == get<1>(flight)) {
                                            truckRouteCost = g->getManhattanDistance(tRoute[l-1]->getID(), get<1>(flight)) + g->getManhattanDistance(get<1>(flight), tRoute[l+1]->getID());
                                            break;
                                        }
                                    }

                                    double gain = truckRouteCost - droneFlightCost;
                                    //cout << "ganho: " << gain << endl;

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
                            //cout << endl << "best flight for client " << client->getID() << ": (" << get<0>(bestClientFlight) << "," << get<1>(bestClientFlight) << "," << get<2>(bestClientFlight) << ")" << endl;
                            bestFlight.push_back(make_tuple(get<1>(bestClientFlight), get<0>(bestClientFlight), get<2>(bestClientFlight), biggestGain, true));
                        }

                    } //else {
                        //cout << "Node " << client->getID() << " can't be attended by drone (is depot or first/last node after/before depot)." << endl;
                    //}
                }
            }
        }

        // sorts best flights by gain
        sortListByGain(&bestFlight);
        
        //cout << "iniciando correção de rotas de drones" << endl;

        // adds best flights to route
        //cout << "best flights size: " << bestFlight.size() << endl;
        for (int j = 0; j < bestFlight.size(); j++) {
            
            // verifies if all best flights are possible
            for (int k = j+1; k < bestFlight.size(); k++) {
                if (get<0>(bestFlight[j]) == get<1>(bestFlight[k]) || get<1>(bestFlight[j]) == get<0>(bestFlight[k])) {
                    //cout << "encontrou erro: (" << get<1>(bestFlight[j]) << "," << get<0>(bestFlight[j]) << "," << get<2>(bestFlight[j]) << ") e (" << get<1>(bestFlight[k]) << "," << get<0>(bestFlight[k]) << "," << get<2>(bestFlight[k]) << ")" << endl;
                    get<4>(bestFlight[k]) = false;
                }
            }

            // if flight is possible, adds it to route
            if (get<4>(bestFlight[j])) {
                get<0>(flight) = get<1>(bestFlight[j]);
                get<1>(flight) = get<0>(bestFlight[j]);
                get<2>(flight) = get<2>(bestFlight[j]);
                sol->getRoute(i)->insertDroneFlight(flight);
                sol->setDroneRouteCreated(true);
            }
        }

        // corrects truck route (removes clients that were served by drone)
        sol->getRoute(i)->removeClientsServedByDrone(g, CT, CD, CB);
        bestFlight.clear();
        searchRange.clear();
    }

    sol->updateSolution(g);
}

void sortListByGain(vector<tuple<int, int, int, double, bool>> *list) {
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

void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode) {
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

bool isReachableByDrone(Graph *g, Solution *sol, tuple<int, int, int> flight, int routeIndex) {
    //cout << endl << endl << "verifying flight: [" << get<0>(flight) << ", " << get<1>(flight) << ", " << get<2>(flight) << "]" << endl;

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
        //cout << "drone time: " << droneTime << endl;

        int nodeID = 0, index = 0;
        while (nodeID != launchNode) {
            index++;
            nodeID = sol->getRoute(routeIndex)->getNode(index)->getID();
        }

        // launch node is in index position in route
        // verifies nodes before retrieve node, besides client node, and calculates time spent
        double truckTime = 0;
        int nextNodeID = 0;

        while (nextNodeID != retrieveNode) {
            nodeID = sol->getRoute(routeIndex)->getNode(index)->getID();
            index++;
            nextNodeID = sol->getRoute(routeIndex)->getNode(index)->getID();
            if (nextNodeID != clientNode) {
                truckTime += g->getManhattanDistance(nodeID, nextNodeID) / ST;  
                //cout << "truck time between " << nodeID << " and " << nextNodeID << ": " << g->getManhattanDistance(nodeID, nextNodeID) / ST << endl;
            }
            else {
                //cout << nextNodeID << " is client! skipping..." << endl;
                index++;
                nextNodeID = sol->getRoute(routeIndex)->getNode(index)->getID();
                truckTime += g->getManhattanDistance(nodeID, nextNodeID) / ST;
                //cout << "truck time between " << nodeID << " and " << nextNodeID << ": " << g->getManhattanDistance(nodeID, nextNodeID) / ST << endl;
            }
        }

        //cout << "total truck time: " << truckTime << endl;

        if (truckTime <= droneTime) {
            //cout << "flight is possible" << endl;
            return true;
        }
    }

    //cout << "flight is not possible" << endl;
    return false;
}

void updateSearchRange(vector<int>*searchRange, int rNode) {
    //cout << endl << "updating search range" << endl;

    int firstNode = searchRange->front();

    while (firstNode != rNode) {
        searchRange->erase(searchRange->begin());
        firstNode = searchRange->front();
    }
}

void printCandidatesCost(Solution *sol) {
    // print candidates list for each route
    for (int j = 0; j < sol->getNumRoutes(); j++) {
        cout << endl;
        cout << "CANDIDATES FOR ROUTE " << j << endl;
        for (int i = 0; i < sol->getCandidatesCost().size(); i++) {
            if (get<1>(sol->getCandidateCost(i)) == j)
                cout << "Candidate " << get<0>(sol->getCandidateCost(i)) << " between nodes " <<  get<3>(sol->getCandidateCost(i)) << " and " << get<4>(sol->getCandidateCost(i)) <<" => delta = " << get<2>(sol->getCandidateCost(i)) << endl;
        }
    }

    cout << "------------------" << endl;
}
