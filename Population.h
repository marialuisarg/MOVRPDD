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
        vector<Solution*> solutions;
        vector<vector<Solution*>> fronts;
        Graph *g;

    public:
        Population(int size, int numClients, Graph *g, int QT);
        Population(int size, int numClients, Graph *g, int QT, double alpha, int numIterations, int constructorType);
        ~Population();
        
        void include(vector<Solution*> sol);
        Solution* decode(vector<int> sol, int QT);

        vector<Solution*> getSolutions();
        void FNDS();                            // Fast Non-Dominated Sort

        void printEncodedSolution(vector<int> sol);
        void printDecodedSolution(Solution *sol);
        void printPopulation();
        void printFronts();

};

#endif // POPULATION_H_INCLUDED