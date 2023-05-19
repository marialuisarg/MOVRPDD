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

int main() {
    Utils u;
    vector<Node> nodes;
    string fileName;
    cout << "Insira o nome do arquivo: ";
    cin >> fileName;

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

    Solution s(&graph, 1000);    
    //s.plotTruckSolution(&s, fileName);
    s.plotSolution(&s, fileName);
    //s.printSolution();

    return 0;
}
