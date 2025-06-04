#include "../include/Graph.hpp"
#include "../include/Types.hpp"
#include "../include/Utils.hpp"
#include "../include/ENSGA2.hpp"
#include "../include/Constructor.hpp"
#include "../include/Solution.hpp"
#include "../include/RandomGenerator.hpp"

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

    if (!(argc == 7 || argc == 6)) {
        std::cout << "ERROR: Expecting <instance_file> <alpha> <population_size> <number_of_iterations> <tournament_size> <seed>" << std::endl;
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
    
    if (argc == 7 || argc == 6) {
        
        // set seed
        unsigned int seed = (argc == 7) ? std::stoul(argv[6]) : std::random_device{}();
        RandomGenerator rng(seed);

        // arguments for random truck route constructor
        float alpha_constructor = atof(argv[2]);         // alpha for random truck route constructor
        int it_constructor = 100;                        // number of iterations for random truck route constructor

        // arguments for ENSGA2
        int population = atoi(argv[3]);                 // population size
        int it_GA = atoi(argv[4]);                      // number of iterations for ENSGA2
        int tournament = atoi(argv[5]);                 // tournament size

        // ENSGA2
        ENSGA2::run(population, numNodes, &graph, alpha_constructor, it_constructor, it_GA, fileName, tournament, &rng);
    }
    
    return 0;
}
