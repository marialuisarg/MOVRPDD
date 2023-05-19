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

int main(int argc, char const *argv[]) {

    if (argc != 3) {
        cout << "ERROR: Expecting: ./<program_name> <instance_file> <QT>" << endl;
        exit(1);
    }

    string fileName = argv[1];
    int QT = atoi(argv[2]);

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

    // truck route
    Solution t(&graph, QT);    
    t.plotSolution(&t, fileName);

    // drone route
    t.createDroneRoutes(&graph);
    t.plotSolution(&t, fileName);

    return 0;
}
