#include "../include/Mutation.hpp"

namespace Mutation {

    std::vector<int> run(vector<int> s, RandomGenerator *rng) {
        int mutationType = getRandomPosition(0, 2, rng);

        if (mutationType == 0) {
            //std::cout << "SWAP" << std::endl;
            return swap(s, rng);
        } else if (mutationType == 1) {
            // std::cout << "INSERT" << std::endl;
            return insert(s, rng);
        } else {
            // std::cout << "REVERSE" << std::endl;
            return reverse(s, rng);
        }
    }

    int getRandomPosition(int begin, int end, RandomGenerator *rng) {
        return rng->getInt(begin, end);
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
    std::vector<int> swap(vector<int> solution, RandomGenerator *rng) {

        int pos1 = getRandomPosition(0, solution.size() - 1, rng);
        int pos2 = 0;

        while (pos1 < 0 || pos1 >= solution.size() || solution[pos1] == 0 || solution[pos1] > solution.size()) {
            pos1 = getRandomPosition(0, solution.size() - 1, rng);
        }

        do {
            pos2 = getRandomPosition(0, solution.size() - 1, rng);
        } while (pos1 == pos2 || pos2 < 0 || pos2 >= solution.size() || solution[pos2] == 0 || solution[pos2] > solution.size());

        std::swap(solution[pos1], solution[pos2]);
        
        //printSolution(solution);
        return solution;
    }

    std::vector<int> swap(vector<int> solution, int pos1, int pos2) {
        std::swap(solution[pos1], solution[pos2]);
        //printSolution(solution);
        return solution;
    }

    // inserts client originally in position 1 before client in position 2
    std::vector<int> insert(vector<int> solution, RandomGenerator *rng) {

        int pos1 = getRandomPosition(0, solution.size() - 1, rng);
        int pos2 = 0;

        while (pos1 < 0 || pos1 >= solution.size() || solution[pos1] == 0 || solution[pos1] > solution.size()) {
            pos1 = getRandomPosition(0, solution.size() - 1, rng);
        }

        do {
            pos2 = getRandomPosition(0, solution.size() - 1, rng);
        } while (pos1 == pos2 || pos2 < 0 || pos2 >= solution.size() || solution[pos2] == 0 || solution[pos2] > solution.size());

        int client;
        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }
        
        client = solution[pos1];
        solution.erase(solution.begin() + pos1);
        solution.insert(solution.begin() + pos2, client);

        //printSolution(solution);
        return solution;
    }

    std::vector<int> insert(vector<int> solution, int pos1, int pos2) {
        int client;
        
        client = solution[pos1];
        solution.erase(solution.begin() + pos1);
        solution.insert(solution.begin() + pos2, client);

        //printSolution(solution);
        return solution;
    }

    // reverses the order of clients between positions 1 and 2, inclusive 
    std::vector<int> reverse(vector<int> solution, RandomGenerator *rng) {

        int pos1 = getRandomPosition(0, solution.size() - 1, rng);
        int pos2 = 0;

        while (pos1 < 0 || pos1 >= solution.size() || solution[pos1] == 0 || solution[pos1] > solution.size()) {
            pos1 = getRandomPosition(0, solution.size() - 1, rng);
        }

        char a;

        do {
            pos2 = getRandomPosition(0, solution.size() - 1, rng);
        } while (pos1 == pos2 || pos2 < 0 || pos2 >= solution.size() || solution[pos2] == 0 || solution[pos2] > solution.size());

        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }

        std::reverse(solution.begin() + pos1, solution.begin() + pos2 + 1);

        //printSolution(solution);
        return solution;
    }

    std::vector<int> reverse(vector<int> solution, int pos1, int pos2) {
        std::reverse(solution.begin() + pos1, solution.begin() + pos2 + 1);
        //printSolution(solution);
        return solution;
    }
}