#include "../include/Constructor.hpp"

namespace Constructor {
    void insertRandomizedFirstClients(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, RandomGenerator *rng) {
        int n = sol->getNumRoutes();
        int i = 0;
        while (i < n) {
            int randomIndex = rng->getInt(0, sol->getCandidatesCost().size()-1);
            
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

    bool isInSearchRange(const deque<long>& searchRange, long int nodeID) {
        return std::find(searchRange.begin(), searchRange.end(), nodeID) != searchRange.end();
    }

    void sortListByEuclideanDistance(Graph *g, vector<int> *nodeIdList, int clientNode) {
        std::sort(nodeIdList->begin(), nodeIdList->end(), [g, clientNode](int a, int b) {
            return g->getEuclideanDistance(clientNode, a) < g->getEuclideanDistance(clientNode, b);
        });
    }

    void sortListByGain(vector<tuple<int, int, int, double, bool>> *list) {
        std::sort(list->begin(), list->end(), [](const auto &a, const auto &b) {
            return get<3>(a) > get<3>(b);
        });
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

        //cout << "expct_max_flight_endurance: " << expct_max_flight_endurance << endl;
        //cout << "time spent: " << t_ij + t_jk << endl;

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
        // for (int j = 0; j < sol->getNumRoutes(); j++) {
        //     cout << endl;
        //     cout << "CANDIDATES FOR ROUTE " << j << endl;
        //     for (int i = 0; i < sol->getCandidatesCost().size(); i++) {
        //         if (get<1>(sol->getCandidateCost(i)) == j)
        //             cout << "Candidate " << get<0>(sol->getCandidateCost(i)) << " between nodes " <<  get<3>(sol->getCandidateCost(i)) << " and " << get<4>(sol->getCandidateCost(i)) <<" => delta = " << get<2>(sol->getCandidateCost(i)) << endl;
        //     }
        // }

        cout << "------------------" << endl;

        // print candidates list in order
        cout << "CANDIDATES LIST" << endl;
        for (int j = 0; j < sol->getCandidatesCost().size(); j++) {
            cout << "Candidate " << get<0>(sol->getCandidateCost(j)) << " (" <<  get<3>(sol->getCandidateCost(j)) << "_" << get<4>(sol->getCandidateCost(j)) <<") => route " << get<1>(sol->getCandidateCost(j)) << endl;
        }
    }

    void createDroneRoutes(Graph *g, Solution *sol) {
        int launchNode, clientNode, retrieveNode;
        vector<int> launchNodesList, retrieveNodesList; 
        tuple<int, int, int> flight(0,0,0); // <launchNode, clientNode, retrieveNode>

        // for each client on each truck route
        for (int i = 0; i < sol->getNumRoutes(); i++) { 
            vector<Node*> tRoute = sol->getRoute(i)->getTruckRoute();
            vector<tuple<int, int, int, double, bool>> bestFlight;  // <launchNode, clientNode, retrieveNode, gain, is possible>
            deque<long int> searchRange;

            // creates a list with all clients on the route (every node besides depot)
            for (int k = 1; k < tRoute.size()-1; k++) {
                searchRange.push_back(tRoute[k]->getID());
            }

            for (int j = 0; j < tRoute.size(); j++) {

                if (isInSearchRange(searchRange, tRoute[j]->getID())) {
                    Node* client = sol->getRoute(i)->getTruckRoute()[j];
                    
                    // if client can be reached by drone and its demand is less than the drone's capacity
                    if (client->getServiceBy() == TRUCK_DRONE && client->getDemand() <= QD) {

                        // verifies if node isn't depot or first/last node on route
                        if ((client->getID() != tRoute[0]->getID()) && (client->getID() != tRoute[1]->getID()) && (client->getID() != tRoute[tRoute.size()-2]->getID())) {
                            //cout << endl << endl << "ROUTE: " << i << " | CLIENT: " << client->getID();
                            clientNode = client->getID();
                            get<1>(flight) = clientNode;

                            // creates a launching nodes list with all nodes before client
                            launchNodesList.clear();
                            for (const auto& nodeID : searchRange) {
                                if (nodeID == clientNode)
                                    break;

                                launchNodesList.push_back(nodeID);
                            }

                            // sort launching nodes list by euclidean distance to client
                            sortListByEuclideanDistance(g, &launchNodesList, clientNode);

                            // creates a retrieving nodes list with all nodes after client
                            retrieveNodesList.clear();
                            auto it = std::find(searchRange.begin(), searchRange.end(), clientNode);
                            if (it != searchRange.end()) {
                                ++it; // Move to the next element after clientNode
                                for (; it != searchRange.end(); ++it) {
                                    retrieveNodesList.push_back(*it);
                                }
                            }

                            // goes through lauching and retrieving nodes list, choosing flights that doesn't exceed flight endurance
                            std::tuple<int,int,int> bestClientFlight; // <launchNode, clientNode, retrieveNode>
                            double biggestGain = -INF;

                            for (int n = 0; n < launchNodesList.size(); n++) {
                                get<0>(flight) = launchNodesList[n];
                                for (int k = 0; k < retrieveNodesList.size(); k++) {
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

                                        if (gain > biggestGain) {
                                            get<0>(bestClientFlight) = get<0>(flight);
                                            get<1>(bestClientFlight) = get<1>(flight);
                                            get<2>(bestClientFlight) = get<2>(flight);

                                            //cout << "best: " << get<0>(bestClientFlight) << " " << get<1>(bestClientFlight) << " " << get<2>(bestClientFlight) << endl;
                                            //cout << "current: " << get<0>(flight) << " " << get<1>(flight) << " " << get<2>(flight) << endl;

                                            biggestGain = gain;
                                        }
                                    }
                                }
                            }
                            
                            // if the best flight is better than the truck route, adds it to the best flight list
                            if (biggestGain > 0) {
                                //cout << endl << "best flight for client " << client->getID() << ": (" << get<0>(bestClientFlight) << "," << get<1>(bestClientFlight) << "," << get<2>(bestClientFlight) << ")" << endl;
                                bestFlight.push_back(make_tuple(get<0>(bestClientFlight), get<1>(bestClientFlight), get<2>(bestClientFlight), biggestGain, true));
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

            unordered_map<int, string> usedNodes; // map to track nodes and their roles in flights

            for (const auto& flightOption : bestFlight) {
                int launchNode = get<0>(flightOption);
                int clientNode = get<1>(flightOption);
                int retrieveNode = get<2>(flightOption);

                // Check if nodes are already used in conflicting roles
                if (usedNodes[clientNode].empty() && 
                    (usedNodes[launchNode].empty() || usedNodes[launchNode] == "retrieve") && 
                    usedNodes[retrieveNode].empty()) {
                    
                    // Add flight to the route
                    tuple<int, int, int> flight = {launchNode, clientNode, retrieveNode};
                    sol->getRoute(i)->insertDroneFlight(flight);
                    sol->setDroneRouteCreated(true);

                    // Mark nodes with their roles
                    usedNodes[launchNode] = "launch";
                    usedNodes[clientNode] = "client";
                    usedNodes[retrieveNode] = "retrieve";
                }
            }

            // corrects truck route (removes clients that were served by drone)
            sol->getRoute(i)->removeClientsServedByDrone(g, CT, CD, CB);
            bestFlight.clear();
            searchRange.clear();
        }

        sol->updateSolution(g);
    }
}

namespace GreedyConstructor {

    void createTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, RandomGenerator *rng) {

        // creates list of candidates
        vector<Node*> candidates;
        vector<tuple<int, int, double, int, int>> candidatesCost;

        // inserts unattended clients into the candidate list
        for (int i = 0; i <= sol->getNumClients(); i++) {
            if (!sol->getAttendedClient(i).second) {
                candidates.push_back(g->getNode(i));
            }
        }

        // calculates cost of each candidate
        for (int r = 0; r < sol->getNumRoutes(); r++) {
            for (int i = 0; i < candidates.size(); i++) {
                double manhattanDistance = g->getManhattanDistance(0, candidates[i]->getID());
                double cost = manhattanDistance * CT * 2;
                candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost, 0, 0));
            }
        }

        sol->setCandidatesCost(candidatesCost);
        //printCandidatesCost(sol);

        // sorts candidates by cost
        sol->sortCandidatesByCost(g);

        //cout << endl;
        Constructor::insertRandomizedFirstClients(g, sol, numRoutes, droneRouteCreated, rng);

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

    Solution* run(Graph *g, int QT, RandomGenerator *randGen) {
        int numRoutes;
        int numClients = g->getSize();
        bool droneRouteCreated;

        vector<pair<int, bool>> attendedClients;

        Solution *sol = new Solution(g, QT, randGen);     // creates solution
        sol->setNumClients(numClients);          // sets number of clients in solution

        for (int i = 0; i < numClients; i++) {
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

        createTruckRoutes(g, sol, &numRoutes, &droneRouteCreated, randGen);
        Constructor::createDroneRoutes(g, sol);
        return sol;
    }
}

namespace RandomConstructor {   

    void createRandomTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, double alpha, RandomGenerator *rng) {

        // creates list of candidates
        vector<Node*> candidates;

        // inserts unattended clients into the candidate list
        for (int i = 0; i <= sol->getNumClients(); i++) {
            if (!sol->getAttendedClient(i).second) {
                candidates.push_back(g->getNode(i));
            }
        }

        vector<tuple<int, int, double, int, int>> candidatesCost;

        // calculates cost of each candidate
        for (int r = 0; r < sol->getNumRoutes(); r++) {
            for (int i = 0; i < candidates.size(); i++) {
                double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
                double cost = manhattanDistance * CT * 2;
                
                candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost,0,0));
            }
        }

        sol->setCandidatesCost(candidatesCost);

        // sorts candidates by cost
        sol->sortCandidatesByCost(g);

        // while there are unattended clients
        while (!sol->allClientsAttended(g)) {
            
            int k = sol->random(0, trunc(alpha * (float)sol->getCandidatesCost().size()));
        
            // gets k candidate 
            tuple<int, int, double, int, int> candidate = sol->getCandidateCost(k);

            int clientID = get<0>(candidate);
            int routeIndex = get<1>(candidate);
            int prevNode = get<3>(candidate);

            Node *client = g->getNode(clientID);

            // if client is not already inserted in a route
            bool ins = sol->includeClient(client, g, prevNode, routeIndex);
        }

        // register final truck routes (before drones)
        for (int i = 0; i < sol->getNumRoutes(); i++) 
            sol->getRoute(i)->registerPrevTruckRoute();

        //printRoutes();
        sol->updateSolution(g);
        // print clients attended or not

    }

    vector<Solution*> run(Graph *g, int QT, double alpha, int numIterations, int setSize, RandomGenerator *rng) {

        int numRoutes;
        bool droneRouteCreated;

        vector<Solution*> popSolutions;

        int n = 0;

        // creates list of clients
        vector<int> clients;
        int numClients = g->getSize()-1;

        for (int i = 0; i <= numClients; i++)
            clients.push_back(g->getNode(i)->getID());  

        while (n < numIterations) {
            Solution *currentSolution = new Solution(g, QT, rng);
            currentSolution->setNumClients(numClients+1);

            // checks if best solutions set is full
            if (popSolutions.size() == setSize) {
                break;
            }

            vector<pair<int,bool>> attendedClients;

            for (int i = 0; i <= numClients; i++) {
                attendedClients.push_back({clients.at(i), false});  
            }

            // depot visited 
            attendedClients[0].second = true;

            currentSolution->setAttendedClients(attendedClients);

            // creates routes
            for (int i = 0; i < currentSolution->getNumRoutes(); i++) {
                Route r(QT, QD, g->getNode(0));
                currentSolution->createRoute(r);
            }

            // creates truck routes
            createRandomTruckRoutes(g, currentSolution, &numRoutes, &droneRouteCreated, alpha, rng);
            cout << "Truck routes created." << endl;
            
            // creates drone routes
            Constructor::createDroneRoutes(g, currentSolution);
            cout << "Drone routes created." << endl << endl;

            popSolutions.push_back(currentSolution);

            n++;
        }

        return popSolutions;
    }
}

namespace AdaptiveConstructor {   

    void createAdaptiveTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated) {
        // Define possíveis valores de alpha
        vector<double> alphaValues = {0.25, 0.5, 0.75, 1.0};

        // cria lista de candidatos
        vector<Node*> candidates;

        // insere clientes não atendidos na lista de candidatos
        for (int i = 0; i <= sol->getNumClients(); i++) {
            if (!sol->getAttendedClient(i).second) {
                candidates.push_back(g->getNode(i));
            }
        }

        vector<tuple<int, int, double, int, int>> candidatesCost;

        // calcula o custo de cada candidato
        for (int r = 0; r < sol->getNumRoutes(); r++) {
            for (int i = 0; i < candidates.size(); i++) {
                double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
                double cost = manhattanDistance * CT * 2;

                candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost, 0, 0));
            }
        }

        sol->setCandidatesCost(candidatesCost);

        // ordena candidatos por custo
        sol->sortCandidatesByCost(g);

        // enquanto houver clientes não atendidos
        while (!sol->allClientsAttended(g)) {
            // Seleciona um alpha aleatoriamente da lista de valores possíveis
            double alpha = alphaValues[sol->random(0, alphaValues.size() - 1)];

            // Determina o índice k baseado no alpha selecionado
            int k = sol->random(0, trunc(alpha * (float)sol->getCandidatesCost().size())-1);

            // Obtém o k-ésimo candidato
            tuple<int, int, double, int, int> candidate = sol->getCandidateCost(k);

            int clientID = get<0>(candidate);
            int routeIndex = get<1>(candidate);
            int prevNode = get<3>(candidate);

            Node *client = g->getNode(clientID);

            // se o cliente ainda não foi inserido em uma rota
            bool ins = sol->includeClient(client, g, prevNode, routeIndex);
        }
        
        // registra rotas finais de caminhão (antes dos drones)
        for (int i = 0; i < sol->getNumRoutes(); i++) 
            sol->getRoute(i)->registerPrevTruckRoute();

        sol->updateSolution(g);
    }

    vector<Solution*> run(Graph *g, int QT, int numIterations, int setSize, RandomGenerator *rng) {
        int numRoutes;
        bool droneRouteCreated;

        vector<Solution*> popSolutions;

        int n = 0;

        // cria lista de clientes
        vector<int> clients;
        int numClients = g->getSize() - 1;

        for (int i = 0; i <= numClients; i++)
            clients.push_back(g->getNode(i)->getID());  

        while (n < numIterations) {
            Solution *currentSolution = new Solution(g, QT, rng);
            currentSolution->setNumClients(numClients + 1);

            // verifica se o conjunto de melhores soluções está cheio
            if (popSolutions.size() == setSize) {
                break;
            }

            vector<pair<int, bool>> attendedClients;

            for (int i = 0; i <= numClients; i++) {
                attendedClients.push_back({clients.at(i), false});  
            }

            // depósito visitado
            attendedClients[0].second = true;

            currentSolution->setAttendedClients(attendedClients);

            // cria rotas
            for (int i = 0; i < currentSolution->getNumRoutes(); i++) {
                Route r(QT, QD, g->getNode(0));
                currentSolution->createRoute(r);
            }

            // cria rotas de caminhão de forma adaptativa
            createAdaptiveTruckRoutes(g, currentSolution, &numRoutes, &droneRouteCreated);
            cout << "Truck routes created adaptively." << endl;
            
            // cria rotas de drone
            Constructor::createDroneRoutes(g, currentSolution);
            cout << "Drone routes created." << endl << endl;

            popSolutions.push_back(currentSolution);

            n++;
        }

        return popSolutions;
    }
}
