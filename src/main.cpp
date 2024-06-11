#include "../include/Graph.hpp"
#include "../include/Types.hpp"
#include "../include/Utils.hpp"
#include "../include/ENSGA2.hpp"
#include "../include/Constructor.hpp"
#include "../include/Solution.hpp"

#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <tuple>
#include <string>

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

    if (!(argc == 5)) {
        cout << "ERROR: Expecting <instance_file> <alpha> <population_size> <number_of_iterations>" << endl;
        exit(1);
    }

    string fileName = argv[1];

    vector<Node> nodes;

    nodes = Util::readFile(fileName);
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
    
    if (argc == 5) {
        
        // arguments for random truck route constructor
        float const_alpha = atof(argv[2]);
        int const_numIterations = 100;

        // arguments for ENSGA2
        int populationSize = atoi(argv[3]);
        int numIterations = atoi(argv[4]);

        // ENSGA2
        ENSGA2::run(populationSize, numNodes, &graph, const_alpha, const_numIterations, numIterations, fileName);
    }
    
    return 0;
}
