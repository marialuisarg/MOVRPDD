#ifndef CROSSOVER_HPP_
#define CROSSOVER_HPP_

#include "Utils.hpp"
#include "RandomGenerator.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_set>

namespace Crossover {   
    std::vector<int> run(Solution *p1, Solution *p2, RandomGenerator *rng);              // run crossover (PMX or OX)
    std::vector<int> PMX(Solution *p1, Solution *p2, RandomGenerator *rng);              // PMX crossover
    std::vector<int> OX(Solution *p1, Solution *p2, RandomGenerator *rng);               // OX crossover
}

#endif // CROSSOVER_HPP_