#include "../include/ENSGA2.hpp"

Solution* ENSGA2::getRandomSolution(Population *p) {
    int randomIndex = rand() % p->getSize();
    return p->getSolutions()[randomIndex];
}

pair<Solution*,Solution*> ENSGA2::tournamentSelection(Population *p, int tournamentSize) {

    // create a priority queue of competitors ordered by rank (lower is better)
    std::priority_queue<competitor, vector<competitor>, comp> competitors;
    std::vector<Solution*> competitorsVector;
    bool alreadyInVector = false;

    // select n different competitors randomly (n = tournamentSize)
    for (int i = 0; i < tournamentSize; i++) {
        Solution *s = getRandomSolution(p);

        double s_f1 = s->getTotalDeliveryCost();
        double s_f2 = s->getTotalEnergyConsumption();
        double s_f3 = s->getTotalDeliveryTime();

        std::cout << "Competitor S" << i+1 << " - Rank: " << s->getRank() << " | f1: " << s_f1 << " | f2: " << s_f2 << " | f3: " << s_f3 << std::endl;

        // if the competitor is already in competitors vector, select another random one
        
        for (auto c : competitorsVector) {
            double c_f1 = c->getTotalDeliveryCost();
            double c_f2 = c->getTotalEnergyConsumption();
            double c_f3 = c->getTotalDeliveryTime();

            if (s_f1 == c_f1 && s_f2 == c_f2 && s_f3 == c_f3) {
                i--;
                std::cout << "Competitor already in tournament pool. Selecting another random competitor." << std::endl;
                
                alreadyInVector = true;
            }

            if (alreadyInVector) break;
        }

        if (!alreadyInVector) competitorsVector.push_back(s);

        alreadyInVector = false;
    }

    // pass competitors vector to priority queue
    for (auto c : competitorsVector) 
        competitors.push(make_pair(c, c->getRank()));

    // select the best two competitors from the tournament
    Solution *bestCompetitor = competitors.top().first;
    competitors.pop();

    return make_pair(bestCompetitor, competitors.top().first);
}

bool ENSGA2::isFeasible(vector<int> solution, Graph *g, int QT) {
    double currentTruckLoad = 0;

    for (int i = 0; i < solution.size(); i++) {

        if (solution[i] == 0 || solution[i] >= g->getSize()) {
            currentTruckLoad = 0;
            continue;
        }

        currentTruckLoad += g->getNode(solution[i])->getDemand();

        if (currentTruckLoad > QT) {
            return false;
        }
    }

    return true;
}

void ENSGA2::run(int popSize, int numNodes, Graph *g, double alpha, int itConstructor, int itGA, string instanceName, int tSize) {
    
    std::cout << "Running ENSGA-II." << std::endl << std::endl;

    // outside loop to control number of generations
    int generation = 1;

    std::cout << "****************************" << std::endl;
    std::cout << "GENERATION 1" << std::endl;
    std::cout << "****************************" << std::endl;
    std::cout << "Creating initial population." << std::endl << std::endl;

    Population p(popSize, numNodes-1, g, QT);
    vector<Solution*> randomSolutions = RandomConstructor::run(g, QT, alpha, itConstructor, popSize);
    p.include(randomSolutions);
    p.FNDS();
    p.printFronts();

    while (generation <= itGA) {
        
        std::cout << "Generating offspring." << std::endl << std::endl;

        // generate set of offspring solutions with crossover and mutation
        Population offspring(popSize, numNodes-1, g, QT);

        while (offspring.getCurrentSize() < popSize) {

            parents prnt = tournamentSelection(&p, tSize);
            vector<int> child;

            // chooses crossover operator (80%) or mutation operator (20%) to generate offspring
            if (rand() % 100 < 80) {
                std::cout << std::endl << "USING CROSSOVER OPERATOR - ";
                child = Crossover::run(prnt.first, prnt.second);
            } else {
                std::cout << std::endl << "USING MUTATION OPERATOR - ";
                child = Mutation::run(prnt.first);
            }

            if (isFeasible(child, g, QT)) {
                std::cout << "New solution is feasible! Including child in offspring" << std::endl;
                offspring.include(offspring.decode(child, QT));
            }

            std::cout << std::endl;            
        }

        std::cout << offspring.getCurrentSize() << " offspring solutions generated." << std::endl;

        std::cout << "-----------------------" << std::endl;
        std::cout << "Performing multi-dimensional search on first front." << std::endl;
        vector<Solution*> setG = multiDimensionalSearch(p.getFront(0));

        // combine population, offspring and setG
        Population newPop(popSize*2, numNodes-1, g, QT);
        newPop.include(p.getSolutions());
        newPop.include(offspring.getSolutions());
        //newPop.include(setG);
        newPop.FNDS();
        newPop.printFronts();

        // select new population
        std::cout << "-----------------------" << std::endl;
        std::cout << "SELECTING NEW POPULATION" << std::endl;
        std::cout << "-----------------------" << std::endl;

        p = Population(popSize, numNodes-1, g, QT);

        int i = 0;

        while (p.getCurrentSize() < popSize) {
            vector<Solution*> currentFront = newPop.getFront(i);

            // if the current front fits in the new population, include it
            if (currentFront.size() + p.getCurrentSize() <= popSize) {
                p.include(newPop.getFront(i));
                i++;
                continue;
            }

            // if not, sort the current front by crowding distance
            newPop.crowdingDistance(currentFront);
            while (p.getCurrentSize() < popSize) {
                p.include(currentFront.back());
                currentFront.pop_back();
            }
        }

        p.FNDS();
        p.printFronts();

        p.saveGeneration(generation, instanceName);
        generation++;

        if (generation > itGA) break;

        std::cout << "****************************" << std::endl;
        std::cout << "GENERATION " << generation << std::endl;
        std::cout << "****************************" << std::endl;
    }

        // char control;
        // std::cin >> control;
}

// based on multi-dimensional local search by (Zhang et al., 2022)
vector<Solution*> ENSGA2::multiDimensionalSearch (vector<Solution*> firstFront) {
    vector<Solution*> newFront;
    
    for (int i = 0; i < firstFront.size(); i++) {
        Solution *s = firstFront[i];
        if (s->getDominatedBy() > 0) continue;

        double objFuncS = 0;
        double objFuncS1 = 0;

        // for each objective function
        for (int obj = 0; obj < 3; obj++) {
            Solution *s1 = new Solution(*s);
            Mutation::run(s1);

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
                newFront.push_back(s1);
            }
        }   
    }

    return newFront;
}