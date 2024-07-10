#include "../include/Mutation.hpp"

namespace Mutation {

    std::vector<int> run(Solution *s) {
        int mutationType = getRandomPosition(0, 2);

        if (mutationType == 0) {
            std::cout << "SWAP" << std::endl;
            return swap(s);
        } else if (mutationType == 1) {
            std::cout << "INSERT" << std::endl;
            return insert(s);
        } else {
            std::cout << "REVERSE" << std::endl;
            return reverse(s);
        }
    }

    int getRandomPosition(int begin, int end) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(begin, end);
        return dis(gen);
    }

    void printSolution(std::vector<int> solution) {
        std::cout << "CHILD: " << std::endl; 

        for (int i = 0; i < solution.size(); i++) {
            std::cout << solution[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "-----------------------" << std::endl;
    }

    // swaps clients in positions 1 and 2 (randomly chosen)
    std::vector<int> swap(Solution *s) {
        std::vector<int> solution = s->encode();
        int numClients = s->getNumClients();

        int pos1 = getRandomPosition(0, solution.size() - 1);
        int pos2 = 0;

        while (pos1 < 0 || pos1 >= solution.size() || solution[pos1] == 0 || solution[pos1] > numClients) {
            pos1 = getRandomPosition(0, solution.size() - 1);
        }

        do {
            pos2 = getRandomPosition(0, solution.size() - 1);
        } while (pos1 == pos2 || pos2 < 0 || pos2 >= solution.size() || solution[pos2] == 0 || solution[pos2] > numClients);

        std::swap(solution[pos1], solution[pos2]);
        
        printSolution(solution);
        return solution;
    }

    // inserts client originally in position 1 before client in position 2
    std::vector<int> insert(Solution *s) {

        std::vector<int> solution = s->encode();
        int numClients = s->getNumClients();

        int pos1 = getRandomPosition(0, solution.size() - 1);
        int pos2 = 0;

        while (pos1 < 0 || pos1 >= solution.size() || solution[pos1] == 0 || solution[pos1] > numClients) {
            pos1 = getRandomPosition(0, solution.size() - 1);
        }

        do {
            pos2 = getRandomPosition(0, solution.size() - 1);
        } while (pos1 == pos2 || pos2 < 0 || pos2 >= solution.size() || solution[pos2] == 0 || solution[pos2] > numClients);

        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }

        int client = solution[pos1];
        solution.erase(solution.begin() + pos1);
        solution.insert(solution.begin() + pos2, client);

        printSolution(solution);
        return solution;
    }

    // reverses the order of clients between positions 1 and 2, inclusive 
    std::vector<int> reverse(Solution *s) {
        std::vector<int> solution = s->encode();
        int numClients = s->getNumClients();

        int pos1 = getRandomPosition(0, solution.size() - 1);
        int pos2 = 0;

        while (pos1 < 0 || pos1 >= solution.size() || solution[pos1] == 0 || solution[pos1] > numClients) {
            pos1 = getRandomPosition(0, solution.size() - 1);
        }

        char a;

        do {
            pos2 = getRandomPosition(0, solution.size() - 1);
        } while (pos1 == pos2 || pos2 < 0 || pos2 >= solution.size() || solution[pos2] == 0 || solution[pos2] > numClients);

        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }

        std::reverse(solution.begin() + pos1, solution.begin() + pos2 + 1);

        printSolution(solution);
        return solution;
    }
}