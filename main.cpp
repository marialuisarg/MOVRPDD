#include <iostream>
#include <stdlib.h>
#include<cstdlib>
#include<ctime>

#include <vector>
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

    // for (int i = 0; i < numSolutions; i++) {
    //     Solution s(&graph, QT);
    //     s.createTruckRoutes(&graph);
    //     s.plotSolution(&s, fileName, i);
    //     s.createDroneRoutes(&graph);
    //     s.plotSolution(&s, fileName, i);
    // }

    // truck route
    Solution t(&graph, QT);    
    t.plotSolution(fileName, 0);

    //printObjFunc(&t);

    // drone route
    t.createDroneRoutes(&graph);
    t.plotSolution(fileName, 0);

    printObjFunc(&t);

    // // truck route
    // Solution v(&graph, QT);    
    // v.plotSolution(&v, fileName, 1);
    // printObjFunc(&v);

    // // drone route
    // v.createDroneRoutes(&graph);
    // v.plotSolution(&v, fileName, 1);
    // printObjFunc(&v);

    // // truck route
    // Solution w(&graph, QT);    
    // w.plotSolution(&w, fileName, 2);
    // printObjFunc(&w);

    // // drone route
    // w.createDroneRoutes(&graph);
    // w.plotSolution(&w, fileName, 2);
    // printObjFunc(&w);

    return 0;
}
