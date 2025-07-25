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

    bool isReachableByDrone(Graph *g, tuple<int, int, int> flight, std::vector<int>* truckRoute) {
        //cout << endl << endl << "verifying flight: [" << get<0>(flight) << ", " << get<1>(flight) << ", " << get<2>(flight) << "]" << endl;
        
        int launchNode = get<0>(flight);
        int clientNode = get<1>(flight);
        int retrieveNode = get<2>(flight);

        double distance_ij = g->getEuclideanDistance(launchNode, clientNode);
        double distance_jk = g->getEuclideanDistance(clientNode, retrieveNode);
        double totalDistance = distance_ij + distance_jk;

        // calculates expected maximum flight endurance
        double loadedFlight = (distance_ij / totalDistance) * g->getDroneFlightEndurance(launchNode, clientNode);
        double emptyFlight = (distance_jk / totalDistance);

        if (loadedFlight <= 0) return false; // drone can't fly to client node

        //std::cout << "o drone é capaz de voar até o cliente" << std::endl;
        double expct_max_flight_endurance = (loadedFlight + emptyFlight)*E;
        
        // if expected maximum flight endurance is greater or equal to the time needed to travel 
        // from launch node to client node and from client node to retrieve node, returns true

        double t_ij = g->getDroneFlightTime(launchNode, clientNode);

        if (t_ij == INF) return false; // drone can't fly to client node

        double t_jk = g->getDroneFlightTime(clientNode, retrieveNode);

        double droneTime = t_ij + t_jk;

        //cout << "expct_max_flight_endurance: " << expct_m
        //cout << "time spent: " << t_ij + t_jk << endl;
        
        if (droneTime > expct_max_flight_endurance) return false; 

        // verifies if truck will be able to reach client node before or with drone
        //cout << "drone time: " << droneTime << endl;

        auto it = std::find(truckRoute->begin(), truckRoute->end(), launchNode);

        double totalTruckTime = 0;
        int currentNodeID = launchNode;
        int nextNodeID;
        int index;
        
        if (it != truckRoute->end()) {
            index = std::distance(truckRoute->begin(), it);             // finds index of launch node
        }

        do {
            index++;
            nextNodeID = truckRoute->at(index);

            if (nextNodeID == clientNode) {
                index++;
                nextNodeID = truckRoute->at(index);
            }

            totalTruckTime += g->getTruckTravelTime(currentNodeID, nextNodeID);


        } while (nextNodeID != retrieveNode);

        //cout << "total truck time: " << truckTime << endl;

        return totalTruckTime <= droneTime;

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

        std::cout << "------------------" << endl;

        // print candidates list in order
        std::cout << "CANDIDATES LIST" << endl;
        for (int j = 0; j < sol->getCandidatesCost().size(); j++) {
            std::cout << "Candidate " << get<0>(sol->getCandidateCost(j)) << " (" <<  get<3>(sol->getCandidateCost(j)) << "_" << get<4>(sol->getCandidateCost(j)) <<") => route " << get<1>(sol->getCandidateCost(j)) << endl;
        }
    }
}

// namespace GreedyConstructor {

//     void createTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, RandomGenerator *rng) {

//         // creates list of candidates
//         vector<Node*> candidates;
//         vector<tuple<int, int, double, int, int>> candidatesCost;

//         // inserts unattended clients into the candidate list
//         for (int i = 0; i <= sol->getNumClients(); i++) {
//             if (!sol->getAttendedClient(i).second) {
//                 candidates.push_back(g->getNode(i));
//             }
//         }

//         // calculates cost of each candidate
//         for (int r = 0; r < sol->getNumRoutes(); r++) {
//             for (int i = 0; i < candidates.size(); i++) {
//                 double manhattanDistance = g->getManhattanDistance(0, candidates[i]->getID());
//                 double cost = manhattanDistance * CT * 2;
//                 candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost, 0, 0));
//             }
//         }

//         sol->setCandidatesCost(candidatesCost);
//         //printCandidatesCost(sol);

//         // sorts candidates by cost
//         sol->sortCandidatesByCost(g);

//         //cout << endl;
//         Constructor::insertRandomizedFirstClients(g, sol, numRoutes, droneRouteCreated, rng);

//         //printRoutes();
//         //printCandidatesCost(sol);

//         // while there are unattended clients
//         while (!sol->allClientsAttended(g)) {

//             // gets cheapest insertion
//             Node *cheapestInsertion = g->getNode(get<0>(sol->getCandidateCost(0)));

//             // gets route index
//             int iRoute = get<1>(sol->getCandidateCost(0));

//             // gets previous node index in route
//             long int prevNode = get<3>(sol->getCandidateCost(0));

    
//             // inserts client in route ("if" logic here is just for debugging, but function must be called)
//             bool clientIn = sol->includeClient(cheapestInsertion, g, prevNode, iRoute);
//                 //cout << "Client " << cheapestInsertion->getID() << " inserted in route " << iRoute << endl;
            
//             //printCandidatesCost();
//             //cout << endl;
//         }
//         // register final truck routes (before drones)
//         for (int i = 0; i < sol->getNumRoutes(); i++) 
//             sol->getRoute(i)->registerPrevTruckRoute();

//         //printRoutes();
//         sol->updateSolution(g);
//     }

//     Solution* run(Graph *g, int QT, RandomGenerator *randGen) {
//         int numRoutes;
//         int numClients = g->getSize();
//         bool droneRouteCreated;

//         vector<pair<int, bool>> attendedClients;

//         Solution *sol = new Solution(g, QT, randGen);     // creates solution
//         sol->setNumClients(numClients);          // sets number of clients in solution

//         for (int i = 0; i < numClients; i++) {
//             attendedClients.push_back({g->getNode(i)->getID(), false});  
//         }

//         sol->setAttendedClients(attendedClients);

//         // depot visited 
//         sol->setAttendedClient(0, true);

//         // creates routes
//         for (int i = 0; i < sol->getNumRoutes(); i++) {
//             Route r(QT, QD, g->getNode(0));
//             sol->createRoute(&r);
//         }

//         createTruckRoutes(g, sol, &numRoutes, &droneRouteCreated, randGen);
//         Constructor::createDroneRoutes(g, sol);
//         return sol;
//     }
// }

// namespace RandomConstructor {   

//     void createRandomTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, double alpha, RandomGenerator *rng) {

//         // creates list of candidates
//         vector<Node*> candidates;

//         // inserts unattended clients into the candidate list
//         for (int i = 0; i <= sol->getNumClients(); i++) {
//             if (!sol->getAttendedClient(i).second) {
//                 candidates.push_back(g->getNode(i));
//             }
//         }

//         vector<tuple<int, int, double, int, int>> candidatesCost;

//         // calculates cost of each candidate
//         for (int r = 0; r < sol->getNumRoutes(); r++) {
//             for (int i = 0; i < candidates.size(); i++) {
//                 double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
//                 double cost = manhattanDistance * CT * 2;
                
//                 candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost,0,0));
//             }
//         }

//         sol->setCandidatesCost(candidatesCost);

//         // sorts candidates by cost
//         sol->sortCandidatesByCost(g);

//         // while there are unattended clients
//         while (!sol->allClientsAttended(g)) {
            
//             int k = sol->random(0, trunc(alpha * (float)sol->getCandidatesCost().size()));
        
//             // gets k candidate 
//             tuple<int, int, double, int, int> candidate = sol->getCandidateCost(k);

//             int clientID = get<0>(candidate);
//             int routeIndex = get<1>(candidate);
//             int prevNode = get<3>(candidate);

//             Node *client = g->getNode(clientID);

//             // if client is not already inserted in a route
//             bool ins = sol->includeClient(client, g, prevNode, routeIndex);
//         }

//         // register final truck routes (before drones)
//         for (int i = 0; i < sol->getNumRoutes(); i++) 
//             sol->getRoute(i)->registerPrevTruckRoute();

//         //printRoutes();
//         sol->updateSolution(g);
//         // print clients attended or not

//     }

//     vector<Solution*> run(Graph *g, int QT, double alpha, int numIterations, int setSize, RandomGenerator *rng) {

//         int numRoutes;
//         bool droneRouteCreated;

//         vector<Solution*> popSolutions;

//         int n = 0;

//         // creates list of clients
//         vector<int> clients;
//         int numClients = g->getSize()-1;

//         for (int i = 0; i <= numClients; i++)
//             clients.push_back(g->getNode(i)->getID());  

//         while (n < numIterations) {
//             Solution *currentSolution = new Solution(g, QT, rng);
//             currentSolution->setNumClients(numClients+1);

//             // checks if best solutions set is full
//             if (popSolutions.size() == setSize) {
//                 break;
//             }

//             vector<pair<int,bool>> attendedClients;

//             for (int i = 0; i <= numClients; i++) {
//                 attendedClients.push_back({clients.at(i), false});  
//             }

//             // depot visited 
//             attendedClients[0].second = true;

//             currentSolution->setAttendedClients(attendedClients);

//             // creates routes
//             for (int i = 0; i < currentSolution->getNumRoutes(); i++) {
//                 Route r(QT, QD, g->getNode(0));
//                 currentSolution->createRoute(&r);
//             }

//             // creates truck routes
//             createRandomTruckRoutes(g, currentSolution, &numRoutes, &droneRouteCreated, alpha, rng);
//             cout << "Truck routes created." << endl;
            
//             // creates drone routes
//             Constructor::createDroneRoutes(g, currentSolution);
//             cout << "Drone routes created." << endl << endl;

//             popSolutions.push_back(currentSolution);

//             n++;
//         }

//         return popSolutions;
//     }
// }

// namespace AdaptiveConstructor {   

//     void createAdaptiveTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated) {
//         // Define possíveis valores de alpha
//         vector<double> alphaValues = {0.25, 0.5, 0.75, 1.0};

//         // cria lista de candidatos
//         vector<Node*> candidates;

//         // insere clientes não atendidos na lista de candidatos
//         for (int i = 0; i <= sol->getNumClients(); i++) {
//             if (!sol->getAttendedClient(i).second) {
//                 candidates.push_back(g->getNode(i));
//             }
//         }

//         vector<tuple<int, int, double, int, int>> candidatesCost;

//         // calcula o custo de cada candidato
//         for (int r = 0; r < sol->getNumRoutes(); r++) {
//             for (int i = 0; i < candidates.size(); i++) {
//                 double manhattanDistance = candidates[i]->manhattanDistance(g->getNode(0));
//                 double cost = manhattanDistance * CT * 2;

//                 candidatesCost.push_back(make_tuple(candidates[i]->getID(), r, cost, 0, 0));
//             }
//         }

//         sol->setCandidatesCost(candidatesCost);

//         // ordena candidatos por custo
//         sol->sortCandidatesByCost(g);

//         // enquanto houver clientes não atendidos
//         while (!sol->allClientsAttended(g)) {
//             // Seleciona um alpha aleatoriamente da lista de valores possíveis
//             double alpha = alphaValues[sol->random(0, alphaValues.size() - 1)];

//             // Determina o índice k baseado no alpha selecionado
//             int k = sol->random(0, trunc(alpha * (float)sol->getCandidatesCost().size())-1);

//             // Obtém o k-ésimo candidato
//             tuple<int, int, double, int, int> candidate = sol->getCandidateCost(k);

//             int clientID = get<0>(candidate);
//             int routeIndex = get<1>(candidate);
//             int prevNode = get<3>(candidate);

//             Node *client = g->getNode(clientID);

//             // se o cliente ainda não foi inserido em uma rota
//             bool ins = sol->includeClient(client, g, prevNode, routeIndex);
//         }

//         // registra rotas finais de caminhão, giantTour e splitResults
//         vector<int> giantTour;
//         vector<int> predecessors;
//         int currentPredIndex = 0;
//         predecessors.push_back(currentPredIndex);

//         // percorre todas as rotas e registra o índice do antecessor do primeiro cliente de cada uma
//         // printa rotas 
//         for (auto & route : sol->getRoutes()) {
//             cout << "Route: ";
//             for (int i = 1; i < route->getTruckRoute().size(); i++) {
//                 cout << route->getTruckRoute()[i]->getID() << " ";
//                 giantTour.push_back(route->getTruckRoute()[i]->getID());
//                 if (i == 1) {
//                     currentPredIndex = giantTour.size() - 1;
//                 }
//                 predecessors.push_back(currentPredIndex);
//             }
//             cout << endl;
//         }

//         sol->updateSolution(g);
//     }

//     vector<Solution*> run(Graph *g, int QT, int numIterations, int setSize, RandomGenerator *rng) {
//         int numRoutes;
//         bool droneRouteCreated;

//         vector<Solution*> popSolutions;

//         int n = 0;

//         // cria lista de clientes
//         vector<int> clients;
//         int numClients = g->getSize() - 1;

//         for (int i = 0; i <= numClients; i++)
//             clients.push_back(g->getNode(i)->getID());  

//         while (n < numIterations) {
//             Solution *currentSolution = new Solution(g, QT, rng);
//             // calculates demand and divides by max load capacity of trucks to get num of routes
//             currentSolution->setNumRoutes(ceil(g->getTotalDemand() / QT));
//             currentSolution->setNumClients(numClients + 1);

//             // verifica se o conjunto de melhores soluções está cheio
//             if (popSolutions.size() == setSize) {
//                 break;
//             }

//             vector<pair<int, bool>> attendedClients;

//             for (int i = 0; i <= numClients; i++) {
//                 attendedClients.push_back({clients.at(i), false});  
//             }

//             // depósito visitado
//             attendedClients[0].second = true;

//             currentSolution->setAttendedClients(attendedClients);

//             // cria rotas
//             for (int i = 0; i < currentSolution->getNumRoutes(); i++) {
//                 Route r(QT, QD, g->getNode(0));
//                 r.insertClient(g->getNode(0), 0); // insere depósito no fim da rota
//                 currentSolution->createRoute(&r);
//             }

//             // cria rotas de caminhão de forma adaptativa
//             createAdaptiveTruckRoutes(g, currentSolution, &numRoutes, &droneRouteCreated);
//             cout << "Truck routes created adaptively." << endl;
            
//             // cria rotas de drone
//             Constructor::createDroneRoutes(g, currentSolution);
//             cout << "Drone routes created." << endl << endl;

//             popSolutions.push_back(currentSolution);

//             n++;
//         }

//         return popSolutions;
//     }
// }

namespace LiteratureConstructor {

    void split(Graph* g, Solution* sol, vector<int> clients) {
        int n = sol->getNumClients();

        std::vector<double> costShortestPath(n+1);
        std::vector<int> predecessorIndex(n+1);

        costShortestPath[0] = 0;
        predecessorIndex[0] = 0;

        for (int i = 1; i <= n; i++) {
            costShortestPath[i] = INF;
        }

        for (int i = 1; i <= n; i++) {

            int j = i;
            double cost, load = 0.0;
            
            do {
                load = load + g->getNode(clients[j])->getDemand();

                if (i == j) {
                    cost = g->getManhattanDistance(0, clients[i]) + g->getManhattanDistance(clients[i], 0);
                } else {
                    cost = cost - (g->getManhattanDistance(clients[j-1], 0) + g->getManhattanDistance(clients[j-1], clients[j]) + g->getManhattanDistance(clients[j], 0));
                }

                // std::cout << "Testando inserir " << clients[j] << " na rota que termina com " << clients[j-1] << ": custo = " << cost << " | load = " << load << std::endl;
                // std::cout << "Custo da rota entre 0 e o " << i-1 << "o nó: " << costShortestPath[i-1] << std::endl;
                // std::cout << "Custo da rota entre 0 e o " << j << "o nó: " << costShortestPath[j] << std::endl;
                // std::cout << costShortestPath[i-1] + cost << std::endl;

                //if (load > QT) std::cout << "ULTRAPASSOU CARGA MÁXIMA" << std::endl;
                if (load <= QT && costShortestPath[i-1] + cost < costShortestPath[j]) {
                    // std::cout << "Rota atualizada. Primeiro da rota que contém o " << clients[j] << ": " << clients[i-1] << std::endl;
                    costShortestPath[j] = costShortestPath[i-1] + cost;
                    predecessorIndex[j] = i-1;
                }

                j++;
            } while (j <= n && load <= QT);
            
        }
        
        sol->setPredecessors(predecessorIndex);
        //sol->setCosts(costShortestPath);
    }

    std::deque<Route*> extract(const std::vector<int>& clients, const std::vector<int>& predecessorIndex, Graph* g) {
        // std::cout << "\n--- Iniciando Algoritmo 2: Reconstruindo as rotas ---" << std::endl;

        std::deque<Route*> routesList;
        int n = clients.size() - 2;
        int i, j = n;

        do {
            Route* currentTrip = new Route(QT, QD, g->getNode(0));
            i = predecessorIndex[j];
            currentTrip->insertClient(g->getNode(0),0); // insere depósito no início da rota

            // insere clientes da rota atual
            for (int k = i + 1; k <= j; k++) {
                currentTrip->insertClient(g->getNode(clients[k]), clients[k]);
            }
            
            routesList.push_front(currentTrip);
            j = i;

        } while (i != 0);

        return routesList;
    }

    Solution* truckRouteSplit(std::vector<int> clients, Graph* g) {

        Solution* solution = new Solution(g, QT, nullptr);
        solution->setNumClients(clients.size());
        solution->setGiantTour(clients);

        // aux vector with depots
        std::vector<int> giantTour(clients.size() + 2);
        std::copy(clients.begin(), clients.end(), giantTour.begin() + 1);
        giantTour[0] = 0;
        giantTour.back() = 0;

        // for (int i = 0; i < giantTour.size(); i++) {
        //     std::cout << giantTour[i] << " ";
        // }
        // std::cout << std::endl;

        split(g, solution, giantTour);

        std::deque<Route*> extractedRoutes = extract(giantTour, solution->getPredecessors(), g);
        
        for (Route* route : extractedRoutes) {
            solution->includeRoute(route);
            solution->calculateTruckEnergyConsumption(g, route);
        }

        solution->updateDecoded(true);
        //solution->calculateObjectives(g);

        std::cout << "Truck routes created. Number of routes: " << solution->getNumRoutes() << std::endl;
        return solution;
    }

    void droneRouteConstructor(Solution* sol, Graph* g) {
        // creates as many flights as possible for each truck route in solution
        int cLaunch, cCustomer, cRetrieval, lastReturnIndex;
        std::tuple<int, int, int> flight;
        std::vector<int>* currentTruckRoute;
        std::vector<int> currentSearchRange;

        for (Route* route : sol->getRoutes()) {

            //std::cout << "Searching for feasible flights in current route..." << std::endl;

            cLaunch = cCustomer = cRetrieval = 0;
            currentTruckRoute = route->getTruckRouteIDs();
            currentSearchRange.assign(currentTruckRoute->begin(), currentTruckRoute->end());
            lastReturnIndex = 0;

            for (int i = 0; i < currentTruckRoute->size() - 1; i++) {
                cCustomer = currentTruckRoute->at(i);

                if (g->getNode(cCustomer)->getServiceBy() == TRUCK_DRONE) {         // if custumer client can be attended by drone
                    if (g->getNode(cCustomer)->getDemand() <= QD) {                 // if custumer client demand doesn't exceed maximum load capacity of the drone

                        //std::cout << "-- cCustomer = " << cCustomer << std::endl;

                        auto it = std::find(currentSearchRange.begin()+1, currentSearchRange.end(), cCustomer);
                        if (it == currentSearchRange.end()) {
                            //std::cout << cCustomer << " not included in current search space." << std::endl;
                            continue;
                        }

                        auto it_begin = currentSearchRange.begin();
                        std::vector<int> launchList(it_begin, it);

                        //std::cout << "-- launchList = ";

                        // for (int c = 0; c < launchList.size(); c++) 
                        //     std::cout << launchList[c] << " ";
                        // std::cout << std::endl;

                        // launch client will be the node with the mininimum euclidean distance from customer client
                        Constructor::sortListByEuclideanDistance(g, &launchList, cCustomer);
                        cLaunch = launchList[0];

                        //std::cout << "-- closest launch node = " << cLaunch << std::endl;

                        it_begin = it + 1;
                        auto it_end = currentSearchRange.end();
                        std::vector<int> retrievalList(it_begin, it_end);


                        //std::cout << "-- searching for best retrieval node " << std::endl;

                        for (int j = 0; j < retrievalList.size(); j++) {
                            cRetrieval = retrievalList[j];
                            flight = make_tuple(cLaunch, cCustomer, cRetrieval);
                            //std::cout << "------> " << cRetrieval << " j = " << j << std::endl;
                            if (Constructor::isReachableByDrone(g, flight, currentTruckRoute)) {
                                route->insertDroneFlight(flight);
                                //std::cout << "------> FLIGHT POSSIBLE!" << std::endl;
                                
                                // search space is now the nodes after retrieval client
                                auto retPosition = std::find(currentSearchRange.begin(), currentSearchRange.end(), cRetrieval);
                                currentSearchRange.erase(currentSearchRange.begin(), retPosition);

                                // std::cout << "current search range updated: ";
                                // for (int k = 0; k < currentSearchRange.size(); k++)
                                //     std::cout << currentSearchRange[k] << " ";

                                // std::cout << std::endl;
                                break;
                            }
                        }
                    }
                }
            }
        }

        cout << "Drone routes created." << endl;
    }

    vector<Solution*> run(Graph *g, int QT, RandomGenerator *randGen, int setSize) {

        std::cout << "Running Literature Constructor..." << std::endl << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------------------------------------" << std::endl << std::endl;

        vector<Solution*> solutions;

        for (int p = 0; p < setSize; p++) {

            // vector of clients without depots
            std::vector<int> clients; 

            // giant truck vector is initialized with clients in random order
            for (int i = 1; i < g->getSize(); i++) {
                clients.push_back(g->getNode(i)->getID());
            }

            std::shuffle(clients.begin(), clients.end(), randGen->getEngine());

            // creates solution with truck route split algorithm
            Solution* solution = truckRouteSplit(clients, g);

            // solution->printRoutes();

            // cria rotas de drone
            LiteratureConstructor::droneRouteConstructor(solution, g);

            solution->printRoutes(); // prints routes for debugging

            solutions.push_back(solution);
        }

        return solutions;
    }
}