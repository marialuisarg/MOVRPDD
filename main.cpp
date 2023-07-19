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

#include "GreedyConstructor.h"
#include "RandomConstructor.h"

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
        cout << endl << "-------------------" << endl;
        cout << "f1: " << (*it)->getTotalEnergyConsumption() << endl;
        cout << "f2: " << (*it)->getTotalDeliveryCost() << endl;
        cout << "f3: " << (*it)->getTotalDeliveryTime() << endl;
        cout << "-------------------" << endl;
    }
}

int main(int argc, char const *argv[]) {

    if (argc != 4) {
        cout << "ERROR: Expecting: <instance_file> <QT> <number_of_solutions>" << endl;
        exit(1);
    }

    string fileName = argv[1];
    int QT = atoi(argv[2]);
    int numExec = atoi(argv[3]);

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

    // set seed to time(0)
    srand(time(0));

    // set of numSolutions solutions
    vector<Solution*> solutions;
    vector<vector<Solution*>> randomSolutions;

    for (int i = 0; i < numExec; i++) {
        solutions.emplace_back(greedyConstructor(&graph, QT));
        solutions[i]->plotSolution(fileName, i);
        
        cout << "SOLUTION " << i << ": ";
        printObjFunc(solutions[i]);
    }

    u.printSolutionsToFile(solutions, fileName);

    // for (int i = 0; i < numExec; i++) {
    //     randomSolutions.emplace_back(RandomConstructor(&graph, QT, 0.5, 20, 10));
    //     //solutions[i]->plotSolution(fileName, i);
        
    //     cout << "SOLUTIONS SET " << i << ": ";
    //     printObjFunc(randomSolutions[i]);
    // }

    return 0;
}
