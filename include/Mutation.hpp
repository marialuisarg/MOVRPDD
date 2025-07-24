#ifndef MUTATION_HPP_
#define MUTATION_HPP_

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

#include "Solution.hpp"

namespace Mutation
{   
    int getRandomPosition(int begin, int end, RandomGenerator *rng);      // get random position in vector

    std::vector<int> swap(vector<int>, RandomGenerator *rng);             // swap mutation
    std::vector<int> insert(vector<int>, RandomGenerator *rng);           // insert mutation
    std::vector<int> reverse(vector<int>, RandomGenerator *rng);          // reverse mutation

    std::vector<int> swap(vector<int>, int pos1, int pos2);               // swap mutation (for local search)
    std::vector<int> insert(vector<int>, int pos1, int pos2);             // insert mutation (for local search)
    std::vector<int> reverse(vector<int>, int pos1, int pos2);            // reverse mutation (for local search)

    std::vector<int> run(vector<int>, RandomGenerator *rng);              // run mutation (random swap, insert or reverse)
    std::vector<int> runLocalSearch(vector<int> s, RandomGenerator *rng); // run mutation for local search
    
    void             printSolution(std::vector<int> solution);
    
}

#endif // MUTATION_HPP_