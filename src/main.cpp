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
#include <time.h>

double timespec_to_seconds(const struct timespec& ts) {
    return ts.tv_sec + (ts.tv_nsec / 1000000000.0);
}

int main(int argc, char const *argv[]) {

    struct timespec start_ts, end_ts;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_ts);

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

    //graph.printGraph();

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
        executionType typeExec = INVALID;
        string typeExecArg = argv[2];
        if (typeExecArg == "LIT") {
            typeExec = LIT;
        } else if (typeExecArg == "LIT+BL") {
            typeExec = LIT_LS;
        } else if (typeExecArg == "ADPT") {
            typeExec = ADPT;
        } else if (typeExecArg == "ADPT+BL") {
            typeExec = ADPT_LS;
        }

        if (typeExec == INVALID) {
            std::cerr << "Erro: Tipo de execução '" << typeExecArg << "' é inválido!" << std::endl;
        }

        int it_constructor = 100;                        // number of iterations for random truck route constructor

        // arguments for ENSGA2
        int population = atoi(argv[3]);                 // population size
        int it_GA = atoi(argv[4]);                      // number of iterations for ENSGA2
        int tournament = atoi(argv[5]);                 // tournament size

        // ENSGA2
        ENSGA2::run(population, numNodes, &graph, typeExec, it_constructor, it_GA, fileName, tournament, &rng);
    }
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_ts);

    double cpu_time_used = timespec_to_seconds(end_ts) - timespec_to_seconds(start_ts);

    std::cout.precision(9);
    std::cout << std::fixed;
    std::cout << "CPU computing time: " << cpu_time_used << std::endl;

    return 0;
}
