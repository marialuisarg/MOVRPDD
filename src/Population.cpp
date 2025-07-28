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

    // vector<Solution*> sol = RandomConstructor::run(g, q, alpha, numIterations, size, rng);
    // include(sol);
};

Population::~Population() {};

void Population::include(std::vector<std::unique_ptr<Solution>>&& solutionsToMove) {

    solutions.reserve(solutions.size() + solutionsToMove.size());

    solutions.insert(
        solutions.end(),
        std::make_move_iterator(solutionsToMove.begin()),
        std::make_move_iterator(solutionsToMove.end())
    );

    solutionsToMove.clear();
    currentSize = solutions.size();
}

int Population::includeOffspring(std::vector<unique_ptr<Solution>> sol, int gen) {
    std::vector<Solution*> pastGenSol;
    pastGenSol.reserve(this->getSolutions().size());

    for (const auto& sol_ptr : this->getSolutions())
        pastGenSol.push_back(sol_ptr.get());

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
                    repeatedSolFile.close(); 
                }
                break;
            }
        }

        if (isUniqueSolution) this->include(std::move(sol[it]));
    }

    return repeatedSolutions;
};

void Population::include(std::unique_ptr<Solution> sol) {
    solutions.push_back(std::move(sol));
    currentSize++;
};

const std::vector<std::unique_ptr<Solution>>& Population::getSolutions() const {
    return solutions;
};

std::vector<std::unique_ptr<Solution>>& Population::getSolutions() {
    return solutions;
}

std::vector<std::unique_ptr<Solution>> Population::takeSolutions() {

    fronts.clear();
    currentSize = 0;
    
    return std::move(solutions);
}

// fast non-dominated sort (Deb, 2002)
void Population::FNDS() {
    vector<Solution*> Fi; 

    for (const auto& solution_ptr : solutions) {        // for each solution p in population
        vector<int> sp;                             // index of solutions that p dominates
        int index = 0;
        int np = 0;                                 // number of solutions that dominate p

        for (const auto& solution2_ptr : solutions) {   // for each solution q in population
            if (solution_ptr->dominates(solution2_ptr.get()))     
                sp.push_back(index);                
            else if (solution2_ptr->dominates(solution_ptr.get()))
                np++;
            
            index++;
        }

        if (np == 0) {
            solution_ptr->setRank(1);
            Fi.push_back(solution_ptr.get());
        }

        solution_ptr->setDominatedSolutions(sp);
        solution_ptr->setDominatedBy(np);
    }

    int i = 1;                  // front counter

    while (Fi.size() != 0) {
        fronts.push_back(Fi);

        vector<Solution*> Q;    // set of solutions that will be included in front i+1

        for (const auto& p : Fi) {   
            vector<int> sp = p->getDominatedSolutions();

            for (int q_idx : sp) {  
                Solution *sol = solutions[q_idx].get();
                sol->setDominatedBy(sol->getDominatedBy() - 1);

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
            cout << "f1: " << solution->getTotalEnergyConsumption();
            cout << " | f2: " << solution->getTotalDeliveryCost();
            cout << " | f3: " << solution->getTotalDeliveryTime();
            cout << " [C.D. = " << solution->getCrDistance() << "]" << endl;
        }

        i++;
        cout << endl;
    }
};