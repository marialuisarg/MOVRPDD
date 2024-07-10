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

    std::vector<int> swap(Solution *s);             // swap mutation
    std::vector<int> insert(Solution *s);           // insert mutation
    std::vector<int> reverse(Solution *s);          // reverse mutation

    std::vector<int> run(Solution *s);         // run mutation (swap, insert or reverse)
    
    void             printSolution(std::vector<int> solution);
    
}

#endif // MUTATION_HPP_