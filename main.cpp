#include <iostream>
#include <stdlib.h>
#include<cstdlib>
#include<ctime>

#include <vector>
#include <tuple>
#include <string>

#include "Graph.h"
#include "Node.h"
#include "Utils.h"
#include "Solution.h"
#include "Population.h"

#include "GreedyConstructor.h"
#include "RandomConstructor.h"

#define GREEDY          0
#define RANDOM_GREEDY   1
#define RANDOM          2

using namespace std;

void printObjFunc(Solution* sol) {
    cout << endl << "-------------------" << endl;
    cout << "f1: " << sol->getTotalEnergyConsumption() << endl;
    cout << "f2: " << sol->getTotalDeliveryCost() << endl;
    cout << "f3: " << sol->getTotalDeliveryTime() << endl;
    cout << "-------------------" << endl;
}

void printObjFunc(vector<Solution*> sol) {
    for (auto it = sol.begin(); it != sol.end(); it++) {
        cout << "-------------------" << endl;
        cout << "f1: " << (*it)->getTotalEnergyConsumption() << endl;
        cout << "f2: " << (*it)->getTotalDeliveryCost() << endl;
        cout << "f3: " << (*it)->getTotalDeliveryTime() << endl;
        cout << "-------------------" << endl;
    }
}

int main(int argc, char const *argv[]) {

    if (!(argc == 4 || argc == 6)) {
        cout << "ERROR: Expecting: <instance_file> <QT> <number_of_solutions>" << endl;
        cout << "OR <instance_file> <QT> <alpha> <number_of_iterations> <number_of_solutions>" << endl;
        exit(1);
    }

    string fileName = argv[1];
    int QT = atoi(argv[2]);
    int numSolutions = atoi(argv[3]);

    Utils u;
    vector<Node> nodes;

    nodes = u.readFile(fileName);
    int numNodes = nodes.size();
    Graph graph(numNodes);
    
    for (int i = 0; i < numNodes; i++) {
        graph.addNode(nodes[i]);
    }

    graph.printGraph();

    for (int i = 0; i < numNodes; i++) {
        for (int j = 0; j < numNodes; j++) {
            graph.addEdge(i, j);
        }
    }

    // set seed
    srand(10);
    cout << "SEED: " << 10 << endl;

    if (argc == 4) {
        
        vector<Solution*> solutions;

        for (int i = 0; i < numSolutions; i++) {
            solutions.emplace_back(greedyConstructor(&graph, QT));
            solutions[i]->plotSolution(fileName, i);
            
            //cout << "SOLUTION " << i << ": ";
            //printObjFunc(solutions[i]);
        }

        u.printSolutionsToFile(solutions, fileName);

    } else if (argc == 6) {
        
        float alpha = atof(argv[3]);
        int numIterations = atoi(argv[4]);
        int setSize = atoi(argv[5]);

        Population p(setSize, numNodes-1, &graph, QT);
        vector<Solution*> randomSolutions = RandomConstructor(&graph, QT, alpha, numIterations, setSize);
        p.include(randomSolutions);
        p.FNDS();

        randomSolutions[0]->plotSolution(fileName, 0);
        //printObjFunc(randomSolutions[i]);
        
        u.printSolutionsToFile(randomSolutions, fileName, "set", true);  
        p.printFronts();
    }

    return 0;
}
