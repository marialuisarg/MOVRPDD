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

using namespace std;

void printObjFunc(Solution* sol) {
    cout << endl << "-------------------" << endl;
    cout << "f1: " << sol->getTotalEnergyConsumption() << endl;
    cout << "f2: " << sol->getTotalDeliveryCost() << endl;
    cout << "f3: " << sol->getTotalDeliveryTime() << endl;
    cout << "-------------------" << endl;
}

int main(int argc, char const *argv[]) {

    if (argc != 4) {
        cout << "ERROR: Expecting: <instance_file> <QT> <number_of_solutions>" << endl;
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

    // set seed to time(0)
    srand(time(0));

    // set of numSolutions solutions
    vector<Solution> solutions;

    for (int i = 0; i < numSolutions; i++) {
        solutions.emplace_back(&graph, QT);
        
        solutions[i].plotSolution(fileName, i);
        solutions[i].createDroneRoutes(&graph);
        solutions[i].plotSolution(fileName, i);

        // cout << "Solution " << i << ": ";
        // printObjFunc(&solutions[i]);
    }

    u.printSolutionsToFile(&solutions, fileName);

    return 0;
}
