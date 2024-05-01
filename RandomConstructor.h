#ifndef RANDOMCONSTRUCTOR_H_INCLUDED
#define RANDOMCONSTRUCTOR_H_INCLUDED

using namespace std;

#include <string>
#include <vector>

#include "Route.h"
#include "Node.h"
#include "Graph.hpp"
#include "Solution.h"

// random constructor
vector<Solution *> RandomConstructor(Graph *g, int QT, double alpha, int numIterations, int setSize);
void createRandomTruckRoutes(Graph *g, Solution *sol, int *numRoutes, bool *droneRouteCreated, double alpha);

#endif // RANDOMCONSTRUCTOR_H_INCLUDED