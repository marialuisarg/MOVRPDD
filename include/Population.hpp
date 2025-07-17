#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "Graph.hpp"
#include "Solution.hpp"
#include "Route.hpp"
#include "Node.hpp"
#include "Constructor.hpp"
#include "Utils.hpp"
#include "RandomGenerator.hpp"
#include <string>
#include <vector>
#include <map>

using namespace std;

class Population {
    private:
        int                         size;
        int                         currentSize;
        int                         numClients;
        vector<Solution*>           solutions;
        vector<vector<Solution*>>   fronts;
        Graph*                      g;
        RandomGenerator*            rng;

    public:
        Population(int size, int numClients, Graph *g, int QT, RandomGenerator *rng);
        Population(int size, int numClients, Graph *g, int QT, double alpha, int numIterations, int constructorType, RandomGenerator *rng);
        ~Population();
        
        void                include(vector<Solution*> sol);
        void                include(Solution* sol);
        int                 includeOffspring(vector<Solution*> sol, int gen);
        Solution*           decode(vector<int> sol, int QT);
        Solution*           decodeLiterature(vector<int> sol, Graph *g);

        vector<Solution*>           getSolutions();
        vector<Solution*>           getFront(int i) { return this->fronts[i]; };
        vector<vector<Solution*>>   getFronts() { return this->fronts; };
        int                         getSize() { return this->size; };
        int                         getCurrentSize() { return this->currentSize; };

        void                FNDS();                                                 // Fast Non-Dominated Sort
        void                crowdingDistance(vector<Solution*> &ndSet);             // Crowding Distance Assignment
        void                sortByObjective(int obj, vector<Solution*> &ndSet);     // Sort population by objective 
        void                cdPopulation();                                         // aux method to calculate crowding distance for all solutions in population

        void                printEncodedSolution(vector<int> sol);
        void                printDecodedSolution(Solution *sol);
        void                printPopulation();
        void                printFronts();

        void                saveGeneration(int generation, string instanceName);     // Save current population to file

};

#endif // POPULATION_H_INCLUDED