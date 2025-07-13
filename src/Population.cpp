#include "Population.hpp"
#include "Constructor.hpp"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <tuple>
#include <algorithm>
#include <unordered_set>

// starts population without routes constructor
Population::Population(int size, int numClients, Graph *g, int q, RandomGenerator *rng) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;
    this->g = g;
    this->rng = rng;
};

// starts population with routes constructor
Population::Population(int size, int numClients, Graph *g, int q, double alpha, int numIterations, int constructorType, RandomGenerator *rng) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;
    this->g = g;
    this->rng = rng;

    vector<Solution*> sol = RandomConstructor::run(g, q, alpha, numIterations, size, rng);
    include(sol);
};

Population::~Population() {};

void Population::include(vector<Solution*> sol) {
    for (auto it = sol.begin(); it != sol.end(); it++) {
        solutions.push_back((*it));
        currentSize++;
    }
};

int Population::includeOffspring(vector<Solution*> sol, int gen) {
    vector<Solution*> pastGenSol = this->getSolutions();
    bool isUniqueSolution = true;
    int repeatedSolutions = 0;

    for (size_t it = 0; it < sol.size(); it++) {
        for (size_t it2 = 0; it2 < pastGenSol.size(); it2++) {
            isUniqueSolution = (sol[it]->getTotalDeliveryCost() != pastGenSol[it2]->getTotalDeliveryCost() ||
                                sol[it]->getTotalDeliveryTime() != pastGenSol[it2]->getTotalDeliveryTime() ||
                                sol[it]->getTotalEnergyConsumption() != pastGenSol[it2]->getTotalEnergyConsumption());
        
            if (!isUniqueSolution) {
                repeatedSolutions++;

                std::ofstream repeatedSolFile("./solutions/repeatedSolutions.txt", std::ios::app);
    
                if (repeatedSolFile.is_open()) {
                    repeatedSolFile <<  "Gen " << gen << " - " << 
                                        sol[it]->getTotalDeliveryCost() << " | " <<
                                        sol[it]->getTotalDeliveryTime() << " | " <<
                                        sol[it]->getTotalEnergyConsumption() << std::endl;
                    repeatedSolFile.close(); // Fechar o arquivo
                }
                break;
            }
        }

        if (isUniqueSolution) this->include(sol[it]);
    }

    return repeatedSolutions;
};

void Population::include(Solution* sol) {
    solutions.push_back(sol);
    currentSize++;
};

vector<Solution*> Population::getSolutions() {
    return solutions;
};

Solution* Population::decode(vector<int> sol, int q) {

    Solution *decodedSol = new Solution(g, QT, rng);
    int numRoutes = decodedSol->getNumRoutes();
    decodedSol->setNumClients(numClients+1);

    // SPLITTING ENCODED SOLUTION INTO ROUTES
    vector<int> currentRoute;
    vector<vector<int>> routes;
    
    for (int num : sol) {
        if (num == 0 || num > numClients) {
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
    Constructor::createDroneRoutes(g, decodedSol);

    return decodedSol;
};

// fast non-dominated sort (Deb, 2002)
void Population::FNDS() {
    vector<Solution*> Fi; 

    for (const auto& solution : solutions) {        // for each solution p in population
        vector<int> sp;                             // index of solutions that p dominates
        int index = 0;
        int np = 0;                                 // number of solutions that dominate p

        for (const auto& solution2 : solutions) {   // for each solution q in population
            if (solution->dominates(solution2))     
                sp.push_back(index);                
            else if (solution2->dominates(solution))
                np++;
            
            index++;
        }

        if (np == 0) {
            solution->setRank(1);
            Fi.push_back(solution);
        }

        solution->setDominatedSolutions(sp);
        solution->setDominatedBy(np);
    }

    int i = 1;                  // front counter

    while (Fi.size() != 0) {
        fronts.push_back(Fi);

        vector<Solution*> Q;    // set of solutions that will be included in front i+1

        for (const auto& p : Fi) {   
            vector<int> sp = p->getDominatedSolutions();

            for (const auto& q : sp) {  
                Solution *sol = solutions[q];
                sol->setDominatedBy(sol->getDominatedBy()-1);

                // if solution is not dominated by any other solution, it belongs to the next front
                if (sol->getDominatedBy() == 0) {
                    sol->setRank(i+1);
                    Q.push_back(sol);
                }
            }
        }

        i++;
        Fi = Q;
    };
}

void Population::sortByObjective(int objective, vector<Solution*> &ndSet) {
    switch (objective) {
    case 0:
        sort(ndSet.begin(), ndSet.end(), [](Solution *a, Solution *b) {
            return a->getTotalEnergyConsumption() < b->getTotalEnergyConsumption();
        });

        break;

    case 1:
        sort(ndSet.begin(), ndSet.end(), [](Solution *a, Solution *b) {
            return a->getTotalDeliveryCost() < b->getTotalDeliveryCost();
        });

        break;
    
    case 2:
        sort(ndSet.begin(), ndSet.end(), [](Solution *a, Solution *b) {
            return a->getTotalDeliveryTime() < b->getTotalDeliveryTime();
        });

        break;

    default:
        break;
    }
}

void Population::crowdingDistance(vector<Solution*> &ndSet) {
    // Crowding Distance (Deb, 2002)
    // given a set of non-dominated solutions, this function calculates the crowding distance of each solution

    int numSolutions = ndSet.size();

    for (const auto &solution : ndSet)
        solution->setCrDistance(0);

    for (int i = 0; i < 3; i++) {
        sortByObjective(i, ndSet);

        ndSet[0]->setCrDistance(INF);
        ndSet[numSolutions-1]->setCrDistance(INF);
        
        double fmin = ndSet[0]->getObjective(i);
        double fmax = ndSet[numSolutions-1]->getObjective(i);

        for (int j = 1; j < numSolutions-1; j++) {
            double distance = ndSet[j]->getCrDistance() + (ndSet[j+1]->getObjective(i) - ndSet[j-1]->getObjective(i));
            distance /= fmax - fmin;
            ndSet[j]->setCrDistance(distance);
        }
    }

}

void Population::cdPopulation() {
    for (auto &front : fronts) {
        crowdingDistance(front);
    }
}

void Population::saveGeneration(int generation, string instanceName) {
    string genName = "gen" + to_string(generation);
    Util::printGenerationToFile(fronts, instanceName, genName, false);
    //Util::printFunctionsByGenerationToFile(fronts, instanceName, genName, false);
    Util::printFirstFrontsToFile(fronts, instanceName, genName);
}

void Population::printFronts() {
    int i = 1;
    cout << "-----------------------" << endl;
    cout << "PARETO FRONTS" << endl;
    cout << "----------------------- " << endl;

    cdPopulation();

    for (const auto& front : fronts) {
        cout << "Front " << i << endl;
        for (const auto& solution : front) {
            cout << "f1: " << solution->getTotalDeliveryCost();
            cout << " | f2: " << solution->getTotalDeliveryTime();
            cout << " | f3: " << solution->getTotalEnergyConsumption();
            cout << " [C.D. = " << solution->getCrDistance() << "]" << endl;
        }

        i++;
        cout << endl;
    }
};