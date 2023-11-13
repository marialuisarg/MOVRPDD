#include "Population.h"
#include "RandomConstructor.h"
#include <iostream>

#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <tuple>
#include <algorithm>

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

// starts population without routes constructor
Population::Population(int size, int numClients, Graph *g, int q) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;
    this->g = g;
};

// starts population with routes constructor
Population::Population(int size, int numClients, Graph *g, int q, double alpha, int numIterations, int constructorType) {
    this->size = size;
    this->numClients = numClients;
    this->currentSize = 0;
    this->g = g;

    vector<Solution*> sol = RandomConstructor(g, q, alpha, numIterations, size);
    include(sol);
};

Population::~Population() {
};

void Population::include(vector<Solution*> sol) {
    for (auto it = sol.begin(); it != sol.end(); it++) {
        solutions.push_back((*it));
        currentSize++;
    }
};

vector<Solution*> Population::getSolutions() {
    return solutions;
};

Solution* Population::decode(vector<int> sol, int q) {

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

void Population::FNDS() {
    // fast non-dominated sort (Deb, 2002)
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
            double distance = ndSet[j+1]->getObjective(i) - ndSet[j-1]->getObjective(i);
            distance /= fmax - fmin;
            distance += ndSet[j]->getCrDistance();
            ndSet[j]->setCrDistance(distance);
        }
    }

}

void Population::cdPopulation() {
    for (auto &front : fronts) {
        crowdingDistance(front);
    }
}

void printCrossover(vector<int> parent1, vector<int> parent2, vector<int> child) {
    cout << "P1: ";
    for (int i = 0; i < parent1.size(); i++) {
        cout << parent1[i] << " ";
    }

    cout << endl;

    cout << "P2: ";
    for (int i = 0; i < parent2.size(); i++) {
        cout << parent2[i] << " ";
    }

    cout << endl;

    cout << "CH: ";
    for (int i = 0; i < child.size(); i++) {
        cout << child[i] << " ";
    }

    cout << endl;
}

vector<int> Population::PMX(Solution *p1, Solution *p2) {
    cout << "----------------" << endl;
    cout << "Executing PMX" << endl;
    cout << "----------------" << endl;

    // encode parents
    vector<int> parent1 = p1->encode();
    vector<int> parent2 = p2->encode();

    // remove depots from parents
    parent1.erase(parent1.begin());
    parent1.erase(parent1.end()-1);
    parent2.erase(parent2.begin());
    parent2.erase(parent2.end()-1);

    // generate random crossover points
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, parent1.size()-1);

    int cp1, cp2 = 0;

    while (parent1[cp1] == 0) 
        cp1 = dis(gen);
    
    while (parent1[cp2] == 0 || cp2 == cp1)
        cp2 = dis(gen);

    if (cp1 > cp2) {
        int aux = cp1;
        cp1 = cp2;
        cp2 = aux;
    }

    cout << "CP1: [" << cp1 << "] " << parent1[cp1] << endl;
    cout << "CP2: [" << cp2 << "] " << parent1[cp2] << endl;

    vector<int> child(parent1.size(), -1);

    // the segment between the two crossover points is copied from parent 1 to child
    for (int i = cp1; i < cp2; i++) {
        child[i] = parent1[i];
    }

    printCrossover(parent1, parent2, child);
    cout << endl;

    // Looking in the same segment positions in parent 2, select each value that hasn't already been copied to the child. For each value:
    for (int i = cp1; i < cp2; i++) {

        int num = parent2[i];

        // if the number has already been copied to child, skip
        if (find(child.begin(), child.end(), num) != child.end()) {
            cout << num << " já está no filho." << endl;
            continue;
        }

        int indexP1 = i, indexP2 = i;
        bool inserted = false;

        while (!inserted) {
            int valueP1 = parent1[indexP1];
            int valueP2 = parent2[indexP2];

            // Locate V in parent 2.
            auto it = find(parent2.begin(), parent2.end(), valueP1);
            int indexP2 = distance(parent2.begin(), it);

            // If the index of this value in Parent 2 is part of the original swath, go to step i. using this value.
            if (indexP2 >= cp1 && indexP2 < cp2) {
                cout << valueP1 << " está na posição " << indexP2 << " no P2 e já está no swatch original."<< endl;
                indexP1 = indexP2;
                continue;
            }

            // If the position isn't part of the original swath, insert Step A's value into the child in this position.
            if (child[indexP2] == -1) {
                cout << valueP1<< " está na posição " << indexP2 << " no P2, vazia no filho. Inserindo " << num << endl;
                child[indexP2] = num;
                printCrossover(parent1, parent2, child);
                inserted = true;
            }
        }
    }

    // copy the remaining genes from parent 2 to child
    for (int i = 0; i < parent2.size(); i++) {
        if (child[i] == -1)
            child[i] = parent2[i];
    }

    // inserting back depots
    child.insert(child.begin(), 0);
    child.insert(child.end(), 0);
    parent1.insert(parent1.begin(), 0);
    parent1.insert(parent1.end(), 0);
    parent2.insert(parent2.begin(), 0);
    parent2.insert(parent2.end(), 0);

    printCrossover(parent1, parent2, child);

    return child;
}

void Population::printDecodedSolution(Solution *sol) {
    cout << "DECODED SOLUTION FUNCTIONS: " << endl;
    cout << "f1: " << sol->getTotalEnergyConsumption();
    cout << " | f2: " << sol->getTotalDeliveryCost();
    cout << " | f3: " << sol->getTotalDeliveryTime() << endl;
};

void Population::printFronts() {
    int i = 1;
    cout << "-----------------------" << endl;
    cout << "PARETO FRONTS" << endl;
    cout << "----------------------- " << endl;

    cdPopulation();

    for (const auto& front : fronts) {
        cout << "Front " << i << endl;
        for (const auto& solution : front) {
            cout << "f1: " << solution->getTotalEnergyConsumption();
            cout << " | f2: " << solution->getTotalDeliveryCost();
            cout << " | f3: " << solution->getTotalDeliveryTime();
            cout << " [C.D. = " << solution->getCrDistance() << "]" << endl;
        }

        i++;
        cout << endl;
    }
};