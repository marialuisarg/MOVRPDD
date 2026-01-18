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
    int getRandomPosition(int begin, int end);

    std::vector<int> swap(vector<int>);             // swap mutation
    std::vector<int> insert(vector<int>);           // insert mutation
    std::vector<int> reverse(vector<int>);          // reverse mutation

    std::vector<int> run(vector<int>);              // run mutation (swap, insert or reverse)
    
    void             printSolution(std::vector<int> solution);
    
}

#endif // MUTATION_HPP_