#ifndef ENSGA2_HPP_
#define ENSGA2_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <queue>

#include "Solution.h"
#include "Population.h"
#include "Graph.hpp"
#include "RandomConstructor.h"

typedef pair<Solution*, int> competitor;
typedef pair<Solution*, Solution*> parents;

struct comp {
    constexpr bool operator()(
        pair<Solution*, int> const& a,
        pair<Solution*, int> const& b)
        const noexcept
    {
        return a.second > b.second;
    }
};

namespace ENSGA2 {

    void            run(int setSize, int numNodes, Graph *g, int q, double alpha, int numIterations);
    Solution*       getRandomSolution(Population *p);
    parents         tournamentSelection(Population *p, int tournamentSize);
    bool            isFeasible(vector<int> solution, Graph *g, int QT);

    
    void run(int setSize, int numNodes, Graph *g, int q, double alpha, int numIterations) {
        
        // generate first population
        std::cout << "GENERATING FIRST POPULATION" << std::endl;
        Population p(setSize, numNodes-1, g, q);
        vector<Solution*> randomSolutions = RandomConstructor(g, q, alpha, numIterations, setSize);
        p.include(randomSolutions);
        p.FNDS();
        p.printFronts();

        std::cout << "-----------------------" << std::endl;
        std::cout << "GENERATING OFFSPRING" << std::endl;
        std::cout << "-----------------------" << std::endl;

        // generate set of offspring solutions with crossover and mutation
        Population offspring(setSize, numNodes-1, g, q);
        while (offspring.getCurrentSize() < setSize) {

            parents prnt = tournamentSelection(&p, 5);
            vector<int> child = offspring.PMX(prnt.first, prnt.second);

            if (isFeasible(child, g, q)) {
                std::cout << "New solution is feasible! Including child in offspring" << std::endl;
                offspring.include(offspring.decode(child, q));
            }

            std::cout << std::endl;            
        }

        // merge parent and offspring populations
        std::cout << offspring.getCurrentSize() << " offspring solutions generated" << std::endl;
        Population newPop(setSize*2, numNodes-1, g, q);
        newPop.include(p.getSolutions());
        newPop.include(offspring.getSolutions());
        newPop.FNDS();
        newPop.printFronts();

        // select new population
        std::cout << "-----------------------" << std::endl;
        std::cout << "SELECTING NEW POPULATION" << std::endl;
        std::cout << "-----------------------" << std::endl;

        p = Population(setSize, numNodes-1, g, q);

        int i = 0;

        while (p.getCurrentSize() < setSize) {
            vector<Solution*> currentFront = newPop.getFront(i);

            // if the current front fits in the new population, include it
            if (currentFront.size() + p.getCurrentSize() <= setSize) {
                p.include(newPop.getFront(i));
                i++;
                continue;
            }

            // if not, sort the current front by crowding distance
            newPop.crowdingDistance(currentFront);
            while (p.getCurrentSize() < setSize) {
                p.include(currentFront.back());
                currentFront.pop_back();
            }
        }

        p.FNDS();
        p.printFronts();
    }

    Solution* getRandomSolution(Population *p) {
        int randomIndex = rand() % p->getSize();
        return p->getSolutions()[randomIndex];
    }

    pair<Solution*,Solution*> tournamentSelection(Population *p, int tournamentSize) {

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

    bool isFeasible(vector<int> solution, Graph *g, int QT) {
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

}

#endif /* ENSGA2_HPP_ */