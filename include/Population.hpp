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
        int                   size;
        int                   currentSize;
        int                   numClients;
        Graph*                g;
        RandomGenerator*      rng;

        std::vector<std::unique_ptr<Solution>>      solutions;
        std::vector<std::vector<Solution*>>         fronts;

    public:
        Population(int size, int numClients, Graph *g, int QT, RandomGenerator *rng);
        Population(int size, int numClients, Graph *g, int QT, double alpha, int numIterations, int constructorType, RandomGenerator *rng);
        ~Population();

        Population(const Population&) = delete;
        Population& operator=(const Population&) = delete;

        Population(Population&&) = default;
        Population& operator=(Population&&) = default;
        
        void                include(std::unique_ptr<Solution> sol);
        void                include(std::vector<std::unique_ptr<Solution>>&& solutionsToMove);
        int                 includeOffspring(vector<unique_ptr<Solution>> sol, int gen);
        Solution*           decodeLiterature(vector<int> sol, Graph *g);

        const std::vector<std::unique_ptr<Solution>>&     getSolutions() const;
        std::vector<std::unique_ptr<Solution>>&           getSolutions();
        std::vector<std::unique_ptr<Solution>>            takeSolutions();

        std::vector<Solution*>                      getFront(int i) { return this->fronts[i]; };
        vector<vector<Solution*>>                   getFronts() { return this->fronts; };

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