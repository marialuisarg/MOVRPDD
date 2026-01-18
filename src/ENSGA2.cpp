#include "../include/ENSGA2.hpp"

Solution* ENSGA2::getRandomSolution(Population *p, RandomGenerator *rng) {
    int randomIndex = rng->getInt(0, p->getSize()-1);
    return p->getSolutions()[randomIndex].get();
}

Solution* ENSGA2::tournamentSelection(Population *p, int tournamentSize, RandomGenerator *rng) {

    // create a priority queue of competitors ordered by rank (lower is better)
    std::priority_queue<competitor, vector<competitor>, comp> competitors;
    std::vector<Solution*> competitorsVector;
    bool alreadyInVector = false;

    // select n different competitors randomly (n = tournamentSize)
    for (int i = 0; i < tournamentSize; i++) {
        Solution *s = getRandomSolution(p, rng);

        double s_f1 = s->getTotalEnergyConsumption();
        double s_f2 = s->getTotalDeliveryCost();
        double s_f3 = s->getTotalDeliveryTime();

        // std::cout<< "Competitor S" << i+1 << " - Rank: " << s->getRank() << " | f1: " << s_f1 << " | f2: " << s_f2 << " | f3: " << s_f3 << std::endl;

        // if the competitor is already in competitors vector, select another random one
        
        // for (auto c : competitorsVector) {
        //     double c_f1 = c->getTotalDeliveryCost();
        //     double c_f2 = c->getTotalEnergyConsumption();
        //     double c_f3 = c->getTotalDeliveryTime();

        //     if (s_f1 == c_f1 && s_f2 == c_f2 && s_f3 == c_f3) {
        //         i--;
        //         std::cout << "Competitor already in tournament pool. Selecting another random competitor." << std::endl;
                
        //         alreadyInVector = true;
        //     }

        //     if (alreadyInVector) break;
        // }

        // if (!alreadyInVector) 
        competitorsVector.push_back(s);

        // alreadyInVector = false;
    }

    // pass competitors vector to priority queue
    for (auto c : competitorsVector) 
        competitors.push(make_pair(c, c->getRank()));

    return competitors.top().first;
}

bool ENSGA2::isFeasible(vector<int> solution, Graph *g, int QT) {
    double currentTruckLoad = 0;

    for (int i = 0; i < solution.size(); i++) {

        if (solution[i] == 0 || solution[i] >= g->getSize()) {
            currentTruckLoad = 0;
            continue;
        }

        currentTruckLoad += g->getNode(solution[i])->getDemand();
        // std::cout<< "Current truck load: " << currentTruckLoad << " | Client: " << solution[i] << std::endl;

        if (currentTruckLoad > QT) {
            return false;
        }
    }

    return true;
}

bool ENSGA2::isFeasibleLiterature(vector<int> solution, Graph *g) {

    auto s = decodeLiterature(solution, g);

    // check if truck routes are feasible

    for (int i = 0; i < s->getNumRoutes(); i++) {
        Route *r = s->getRoute(i);
        double currentTruckLoad = 0;
        for (int j = 0; j < r->getTruckRoute().size(); j++) {
            currentTruckLoad += r->getTruckRoute()[j]->getDemand();
            // std::cout << "Current truck load: " << currentTruckLoad << " | Client: " << r->getTruckRoute()[j]->getID() << std::endl;
            if (currentTruckLoad > QT) {
                // std::cout << "Truck load exceeded capacity in route " << i << std::endl;
                return false;
            }
        }
    }   

    return true;

}

bool checkChildrenSimilarity(int chromossomeSize, vector<int> child, vector<int> p1, int numPai) {
    int sameAsParent1 = 0;

    for (int i = 0; i < chromossomeSize-1; i++) {
        if (child[i] == p1[i] && child[i+1] == p1[i+1]) sameAsParent1++;
    }

    //std::cout << (100 * sameAsParent1) / (chromossomeSize-1) << "% similar to parent " << numPai <<std::endl;

    return ((100 * sameAsParent1) / (chromossomeSize-1)==100);
}

std::unique_ptr<Solution> ENSGA2::decodeLiterature(vector<int> sol, Graph *g) {
    
    std::unique_ptr<Solution> s_ptr = LiteratureConstructor::truckRouteSplit(sol, g);
    Solution *s = s_ptr.get();
    LiteratureConstructor::droneRouteConstructor(s, g);
    s->calculateObjectiveFunctions(g);

    return s_ptr;
}

// based on multi-dimensional local search by (Zhang et al., 2022)
vector<std::unique_ptr<Solution>> ENSGA2::multiDimensionalSearch (vector<Solution*> firstFront, Graph *g, RandomGenerator *rng) {

    vector<std::unique_ptr<Solution>> newFront;
    
    for (Solution* s : firstFront) {

        if (s->getDominatedBy() > 0) continue;

        double objFuncS = 0;
        double objFuncS1 = 0;

        // for each objective function
        for (int obj = 0; obj < 3; obj++) {

            // Create a new solution by decoding the chromosome again
            auto s1 = decodeLiterature(Mutation::run(s->getGiantTour(), rng), g);

            switch (obj) {
                case 0:
                    objFuncS = s->getTotalEnergyConsumption();
                    objFuncS1 = s1->getTotalEnergyConsumption();
                    break;
                case 1:
                    objFuncS = s->getTotalDeliveryCost();
                    objFuncS1 = s1->getTotalDeliveryCost();
                    break;
                case 2:
                    objFuncS = s->getTotalDeliveryTime();
                    objFuncS1 = s1->getTotalDeliveryTime();
                    break;
                default:
                    break;
            }

            if (objFuncS1 < objFuncS) {
                cout << "New solution is better than the original for F" << obj+1 << ". Including in new front." << endl;
                newFront.push_back(std::move(s1));
            }
        }   
    }

    return newFront;
}

std::unique_ptr<Solution> ENSGA2::applyLocalSearch(Solution *s, Graph *g, RandomGenerator *rng, std::function<double(Solution*)> objectiveGetter) {
    // choose a random mutation operator
    int mutationType = rng->getInt(0, 2); // 0: swap, 1: insert, 2: reverse
    
    std::vector<int> mutatedSolutionGT;
    std::unique_ptr<Solution> bestMutant = nullptr;
    double bestCurrentObjValue = objectiveGetter(s);
    int numClients = s->getGiantTour().size();

    // apply the mutation operator to each combination of clients in the giant tour
    for (int i = 0; i < numClients-1; i++) {
        for (int j = i+1; j < numClients-1; j++) {
            switch (mutationType) {
                case 0: // swap
                    mutatedSolutionGT = Mutation::swap(s->getGiantTour(), i, j);
                    break;
                case 1: // insert
                    mutatedSolutionGT = Mutation::insert(s->getGiantTour(), i, j);
                    break;
                case 2: // reverse
                    mutatedSolutionGT = Mutation::reverse(s->getGiantTour(), i, j);
                    break;
                default:
                    break;
            }

            auto mutatedSolution = decodeLiterature(mutatedSolutionGT, g);

            double mutatedObjValue = objectiveGetter(mutatedSolution.get());

            if (mutatedObjValue < bestCurrentObjValue) {
                //std::cout << "Found a better solution with local search: " << mutatedObjValue << " < " << bestCurrentObjValue << std::endl;
                s->setLocalSearch(true);
                bestMutant = std::move(mutatedSolution);
                bestCurrentObjValue = mutatedObjValue;
            }
        }
    }

    // if a better solution was found, return it
    return bestMutant;
}

vector<std::unique_ptr<Solution>> ENSGA2::massiveLocalSearch(Population *offspring, Graph *g, RandomGenerator *rng) {
    
    vector<std::unique_ptr<Solution>> mutantSolutions;

    // selects two best solutions from offspring (for each objective function) and applies local search to them
    for (int func = 0; func < 3; func++) {
        std::priority_queue<competitorDouble, vector<competitorDouble>, comp> bestSolutions;

        std::function <double(Solution*)> objectiveGetter;

        switch (func)
        {
        case 0: // total energy consumption
            objectiveGetter = [](Solution* s) { return s->getTotalEnergyConsumption(); };
            break;

        case 1: // total delivery cost
            objectiveGetter = [](Solution* s) { return s->getTotalDeliveryCost(); };
            break;
            
        case 2: // total delivery time
            objectiveGetter = [](Solution* s) { return s->getTotalDeliveryTime(); };    
            break;
        
        default:
            break;
        }

        for (const auto& s_ptr : offspring->getSolutions()) {
            double currentValue = objectiveGetter(s_ptr.get());
            bestSolutions.push(make_pair(s_ptr.get(), currentValue));
        }

        // get the two best solutions
        for (int i = 0; i < 2; i++) {
            Solution *best = bestSolutions.top().first;
            bestSolutions.pop();

            if (!best->wasLocalSearchUsed()) {
                auto newBestSolution = applyLocalSearch(best, g, rng, objectiveGetter);

                if (newBestSolution != nullptr) {
                    if (isFeasibleLiterature(newBestSolution->getGiantTour(), g)) {
                        //std::cout << "Best solution found with local search is feasible. Including in mutant solutions." << std::endl;
                        best->setLocalSearch(true);
                        mutantSolutions.push_back(std::move(newBestSolution));
                    } else {
                        //std::cout << "Best solution found with local search is not feasible. Skipping." << std::endl;
                        i--;
                        continue;
                    }
                }
            } else {
                //std::cout << "Solution already used local search. Skipping." << std::endl;
                i--; // decrement i to try again with another solution
            }
        }
    }

    return mutantSolutions;
}

bool areSolutionsEqual(Solution* s1, Solution* s2) {
    if (s1 == s2) return true;
    
    return s1->getTotalDeliveryCost() == s2->getTotalDeliveryCost() &&
           s1->getTotalDeliveryTime() == s2->getTotalDeliveryTime() &&
           s1->getTotalEnergyConsumption() == s2->getTotalEnergyConsumption();
}

bool isPopulationHomogeneous(Population& p) {
    if (p.getCurrentSize() <= 1) {
        return true; // População trivialmente homogênea
    }

    Solution* firstSolution = p.getSolutions()[0].get();

    // Compara a primeira solução com todas as outras
    for (int i = 1; i < p.getCurrentSize(); ++i) {
        if (!areSolutionsEqual(firstSolution, p.getSolutions()[i].get())) {
            return false; // Encontrou uma solução diferente, não é homogênea
        }
    }

    // Se o loop terminar, todas as soluções são idênticas
    return true;
}

void ENSGA2::run(int popSize, int numNodes, Graph *g, executionType typeExec, int itConstructor, int itGA, string instanceName, int tSize, RandomGenerator *rng) {
    
    std::cout << "Running ENSGA-II." << std::endl << std::endl;
    std::cout << "Using seed: " << rng->getSeed() << std::endl;

    // outside loop to control number of generations
    int generation = 1;

    std::cout << "****************************" << std::endl;
    std::cout << "GENERATION 1" << std::endl;
    std::cout << "****************************" << std::endl;
    std::cout << "Creating initial population." << std::endl << std::endl;

    Population p(popSize, numNodes-1, g, QT, rng);
    
    if (typeExec == LIT || typeExec == LIT_LS) {
        auto literatureSolutions = LiteratureConstructor::run(g, QT, rng, popSize);
        for (auto& sol : literatureSolutions) {
            p.include(std::move(sol));
        }
    } else if (typeExec == ADPT || typeExec == ADPT_LS) {
        auto adaptiveSolutions = AdaptiveConstructor::run(g, QT, itConstructor, popSize, rng);
        for (auto& sol : adaptiveSolutions) {
            p.include(std::move(sol));
        }
    }

    p.FNDS();
    p.printFronts();

    while (generation <= itGA) {
        
        // std::cout<< "Generating offspring." << std::endl << std::endl;

        // generate set of offspring solutions with crossover and mutation
        Population offspring(popSize, numNodes-1, g, QT, rng);

        bool populationIsHomogeneous = isPopulationHomogeneous(p);

        if (populationIsHomogeneous) {
            std::cout << "Population converged, depending on mutation for diversity." << std::endl;
        }

        while (offspring.getCurrentSize() < popSize) {

            Solution* prnt1 = tournamentSelection(&p, tSize, rng);
            Solution* prnt2;

            if (populationIsHomogeneous) {
                // a população é homogênea (não há como encontrar um pai diferente)
                prnt2 = prnt1;
            } else {
                // a população tem diversidade (procuramos por um pai diferente)
                int tryCount = 0;
                do {
                    prnt2 = tournamentSelection(&p, tSize, rng);
                } while (areSolutionsEqual(prnt1, prnt2)); // Usando a nova função auxiliar
            }

            vector<int> child1, child2;

            // faz cruzamento em 80% das vezes, depois faz mutação em 20% das vezes
 
            if (rng->getDouble(0,100) < 80) {
                //std::cout<< std::endl << "USING CROSSOVER OPERATOR - ";
                child1 = Crossover::run(prnt1, prnt2, rng);
                //std::cout<< std::endl << "USING CROSSOVER OPERATOR - ";
                child2 = Crossover::run(prnt2, prnt1, rng);

                //std::cout << rng->getDouble(0,100) << "% chance of mutation." << std::endl;
                if (rng->getDouble(0,100) < 20) {
                    //std::cout<< std::endl << "USING MUTATION OPERATOR - ";
                    child1 = Mutation::run(child1, rng);
                }

                if(!checkChildrenSimilarity(child1.size(), child1, prnt1->getGiantTour(), 1)) {
                    if(!checkChildrenSimilarity(child1.size(), child1, prnt2->getGiantTour(), 2)) {
                        // std::cout<< "Including child 1 in offspring" << std::endl;
                        offspring.include(decodeLiterature(child1, g));
                    } else {
                        // std::cout<< "New solution will not be included in offspring because it is 100% equal to parent 2." << std::endl;
                    }
                } else {
                    // std::cout<< "New solution will not be included in offspring because it is 100% equal to parent 1." << std::endl;
                }

                if (rng->getDouble(0,100) < 20) {
                    // std::cout<< std::endl << "USING MUTATION OPERATOR - ";
                    child2 = Mutation::run(child2, rng);
                }

                if(!checkChildrenSimilarity(child2.size(), child2, prnt1->getGiantTour(), 1)) {
                    if(!checkChildrenSimilarity(child2.size(), child2, prnt2->getGiantTour(), 2)) {
                        // std::cout<< "Including child 2 in offspring" << std::endl;
                        offspring.include(decodeLiterature(child2, g));
                    } else {
                        // std::cout<< "New solution will not be included in offspring because it is 100% equal to parent 2." << std::endl;
                    } 
                } else {
                    // std::cout<< "New solution will not be included in offspring because it is 100% equal to parent 1." << std::endl;
                }

            } else {
                // std::cout<< "No crossover or mutation were performed." << std::endl;
            }

            // std::cout<< std::endl;            
        }

        //std::cout<< offspring.getCurrentSize() << " offspring solutions generated." << std::endl;

        // std::cout << "-----------------------" << std::endl;
        // std::cout << "Performing multi-dimensional search on first front." << std::endl;
        // auto setG = multiDimensionalSearch(p.getFront(0));

        if (typeExec == LIT_LS || typeExec == ADPT_LS) {
            auto setM = massiveLocalSearch(&offspring, g, rng);
            // std::cout<< setM.size() << " solutions were improved with local search." << std::endl;
            for (auto& sol : setM) {
                offspring.include(std::move(sol));
            }
        } else {
            // std::cout<< "No local search was performed." << std::endl;
        }

        // combine population, offspring and setM
        Population newPop(popSize + offspring.getCurrentSize(), numNodes-1, g, QT, rng);
        newPop.include(p.takeSolutions());
        newPop.include(offspring.takeSolutions());
        newPop.FNDS();
        //newPop.printFronts();

        // select new population
        // std::cout<< "-----------------------" << std::endl;
        // std::cout<< "SELECTING NEW POPULATION" << std::endl;
        // std::cout<< "-----------------------" << std::endl;

        p = Population(popSize, numNodes-1, g, QT, rng);

        auto& sourceSolutions = newPop.getSolutions();
        
        int front_idx = 0;

        while (p.getCurrentSize() < popSize) {
            vector<Solution*> currentFront = newPop.getFront(front_idx);

            if (currentFront.size() + p.getCurrentSize() <= popSize) {
                for (Solution* survivor_ptr : currentFront) {

                    auto it = std::find_if(sourceSolutions.begin(), sourceSolutions.end(),
                        [&](const auto& u_ptr) { return u_ptr.get() == survivor_ptr; });

                    if (it != sourceSolutions.end()) {
                        p.include(std::move(*it)); 
                    }
                }
            } else {
                newPop.crowdingDistance(currentFront); 
                
                for (Solution* survivor_ptr : currentFront) {
                    if (p.getCurrentSize() >= popSize) break; 

                    auto it = std::find_if(sourceSolutions.begin(), sourceSolutions.end(),
                        [&](const auto& u_ptr) { return u_ptr.get() == survivor_ptr; });
                    
                    if (it != sourceSolutions.end()) {
                        p.include(std::move(*it));
                    }
                }
            }
            front_idx++;
        }

        sourceSolutions.erase(
            std::remove(sourceSolutions.begin(), sourceSolutions.end(), nullptr),
            sourceSolutions.end()
        );

        p.FNDS();
        //p.printFronts();

        p.saveGeneration(generation, instanceName);
        generation++;

        if (generation > itGA) break;

        // std::cout<< "****************************" << std::endl;
        // std::cout<< "GENERATION " << generation << std::endl;
        // std::cout<< "****************************" << std::endl;
    }

        // char control;
        // std::cin >> control;
}