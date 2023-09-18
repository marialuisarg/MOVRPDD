#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "Graph.h"
#include "Solution.h"
#include "Route.h"
#include "Node.h"
#include "GreedyConstructor.h"
#include <string>
#include <vector>

using namespace std;

class Population {
    private:
        int size;
        int currentSize;
        int numClients;
        vector<vector<int>> solutions;

    public:
        Population(int size, int numClients, Graph *g, int QT);
        ~Population();
        
        void include(vector<Solution*> sol, Graph *g);
        Solution* decode(vector<int> sol, Graph *g, int QT);

        void printEncodedSolution(vector<int> sol);
        void printDecodedSolution(Solution *sol);
        void printPopulation();

};

#endif // POPULATION_H_INCLUDED