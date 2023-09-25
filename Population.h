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
        vector<vector<vector<int>>> fronts;

    public:
        Population(int size, int numClients, Graph *g, int QT);
        Population(int size, int numClients, Graph *g, int QT, double alpha, int numIterations, int constructorType);
        ~Population();
        
        void include(vector<Solution*> sol, Graph *g);
        Solution* decode(vector<int> sol, Graph *g, int QT);

        vector<vector<int>> getSolutions();
        void FNDS(Graph *g);                            // Fast Non-Dominated Sort

        void printEncodedSolution(vector<int> sol);
        void printDecodedSolution(Solution *sol);
        void printPopulation();

};

#endif // POPULATION_H_INCLUDED