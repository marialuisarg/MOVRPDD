#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "Solution.h"
#include "Route.h"
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
        Population(int size, int numClients);
        ~Population();
        
        void include(vector<Solution*> sol);
        vector<int> encode(Solution *sol);
        Solution* decode(vector<int> sol);
        void printEncodedSolution(vector<int> sol);
        void printDecodedSolution(Solution *sol);
        void printPopulation();

};

#endif // POPULATION_H_INCLUDED