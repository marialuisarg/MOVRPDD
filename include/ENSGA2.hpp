#ifndef ENSGA2_H_
#define ENSGA2_H_

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <random>

#include "Population.hpp"
#include "Crossover.hpp"
#include "Mutation.hpp"
#include "RandomGenerator.hpp"

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

    void                run(int popSize, int numNodes, Graph *g, double alpha, int itConstructor, int itGA, string instanceName, int tSize, RandomGenerator *rng);
    Solution*           getRandomSolution(Population *p, RandomGenerator *rng);
    Solution*           tournamentSelection(Population *p, int tournamentSize, RandomGenerator *rng);
    bool                isFeasible(vector<int> solution, Graph *g, int QT);
    vector<Solution*>   multiDimensionalSearch (vector<Solution*> firstFront);
}

#endif /* ENSGA2_H_ */
