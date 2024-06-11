#include "../include/ENSGA2.hpp"

Solution* ENSGA2::getRandomSolution(Population *p) {
    int randomIndex = rand() % p->getSize();
    return p->getSolutions()[randomIndex];
}

pair<Solution*,Solution*> ENSGA2::tournamentSelection(Population *p, int tournamentSize) {

    // create a priority queue of competitors ordered by rank (lower is better)
    std::priority_queue<competitor, vector<competitor>, comp> competitors;

    for (int i = 0; i < tournamentSize; i++) {
        Solution *s = getRandomSolution(p);
        std::cout << "Rank of competitor " << i << ": " << s->getRank() << std::endl;
        competitors.push(make_pair(s, s->getRank()));
    }

    // return the best competitor
    std::cout << "Best competitor: " << competitors.top().second << std::endl;

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

void ENSGA2::run(int popSize, int numNodes, Graph *g, double alpha, int itConstructor, int itGA, string instanceName) {
    
    std::cout << "Running ENSGA-II" << std::endl;

    // outside loop to control number of generations
    int generation = 0;

    while (generation < itGA) {
        generation++;
        std::cout << "*****************************************************" << std::endl;
        std::cout << "GENERATION " << generation << std::endl;
        std::cout << "*****************************************************" << std::endl;
    
        Population p(popSize, numNodes-1, g, QT);
        vector<Solution*> randomSolutions = RandomConstructor::run(g, QT, alpha, itConstructor, popSize);
        p.include(randomSolutions);
        p.FNDS();
        p.printFronts();

        std::cout << "-----------------------" << std::endl;
        std::cout << "GENERATING OFFSPRING" << std::endl;
        std::cout << "-----------------------" << std::endl;

        // generate set of offspring solutions with crossover and mutation
        Population offspring(popSize, numNodes-1, g, QT);
        while (offspring.getCurrentSize() < popSize) {

            parents prnt = tournamentSelection(&p, 5);
            vector<int> child;

            // chooses crossover operator (80%) or mutation operator (20%) to generate offspring
            if (rand() % 100 < 80) {
                std::cout << std::endl << "USING CROSSOVER OPERATOR - ";
                child = Crossover::run(prnt.first, prnt.second);
            } else {
                std::cout << std::endl << "USING MUTATION OPERATOR - ";
                child = Mutation::mutation(prnt.first);
            }

            if (isFeasible(child, g, QT)) {
                std::cout << "New solution is feasible! Including child in offspring" << std::endl;
                offspring.include(offspring.decode(child, QT));
            }

            std::cout << std::endl;            
        }

        // merge parent and offspring populations
        std::cout << offspring.getCurrentSize() << " offspring solutions generated" << std::endl;
        Population newPop(popSize*2, numNodes-1, g, QT);
        newPop.include(p.getSolutions());
        newPop.include(offspring.getSolutions());
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
    }
}