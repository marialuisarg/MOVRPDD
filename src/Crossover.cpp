#include "../include/Crossover.hpp"

template<typename T>
bool contains(const std::unordered_set<T>& set, const T& value) {
    return set.find(value) != set.end();
}

void printCrossover(vector<int> parent1, vector<int> parent2, vector<int> child) {
    cout << "P1: ";
    for (int i = 0; i < parent1.size(); i++) {
        cout << parent1[i] << " ";
    }

    cout << endl;

    cout << "P2: ";
    for (int i = 0; i < parent2.size(); i++) {
        cout << parent2[i] << " ";
    }

    cout << endl;

    cout << "CH: ";
    for (int i = 0; i < child.size(); i++) {
        cout << child[i] << " ";
    }

    cout << endl;
}

void checkMissingOrDuplicatedClients(int chromossomeSize, vector<int> child) {
    map<int, int> clients;

    for (int i = 0; i < chromossomeSize; i++) {
        if (clients.find(child[i]) == clients.end()) {
            clients[child[i]] = 1;
        } else {
            clients[child[i]]++;
        }
    }

    if (clients.size() == chromossomeSize) {
        std::cout << std::endl <<  "No client is missing or duplicated!" << std::endl;
    }

    for (const auto& client : clients) {
        if (client.second > 1) {
            std::cout << "Client " << client.first << " appears " << client.second << " times" << std::endl;
        }
    }
}

bool checkChildrenSimilarity(int chromossomeSize, vector<int> child, vector<int> p1) {
    int sameAsParent1 = 0;

    for (int i = 0; i < chromossomeSize-1; i++) {
        if (child[i] == p1[i] && child[i+1] == p1[i+1]) sameAsParent1++;
    }

    std::cout << (100 * sameAsParent1) / (chromossomeSize-1) << "% igual ao pai" << std::endl;

    return ((100 * sameAsParent1) / (chromossomeSize-1)==100);
}

std::vector<int> Crossover::run(Solution *p1, Solution *p2, RandomGenerator *rng) {
    
    int crossoverOp = rng->getDouble(0,100);
    
    // choose crossover operator
    if (crossoverOp < 50) {
        std::cout << "PMX" << std::endl;
        return Crossover::PMX(p1, p2, rng);
    } else {
        std::cout << "OX" << std::endl;
        return Crossover::OX(p1, p2, rng);
    }
}

std::vector<int> Crossover::PMX(Solution *p1, Solution *p2, RandomGenerator *rng) {
    // encode parents
    // std::vector<int> parent1 = p1->encode();
    // std::vector<int> parent2 = p2->encode();

    vector<int> parent1 = p1->getGiantTour();
    vector<int> parent2 = p2->getGiantTour();

    int cromossomeSize = parent1.size();

    int cp2 = 0, cp1 = rng->getInt(0, cromossomeSize-1);
    
    while (cp2 == cp1 || cp2 == 0)
        cp2 = rng->getInt(0, cromossomeSize-1);

    if (cp1 > cp2) 
        std::swap(cp1, cp2);

    vector<int> child = parent2;
    vector<bool> is_direct(cromossomeSize, false);

    for (size_t i = cp1; i < cp2; i++) {
        child[i] = parent1[i];
        is_direct[parent1[i]] = true;
    }

    vector<int> index_lookup(cromossomeSize, 0);

    for (size_t i = 0; i < cromossomeSize; i++) {
        index_lookup[parent2[i]] = i;
    }

    for (size_t i = cp1; i < cp2; i++) {
        if (!is_direct[parent2[i]]) {
            size_t pos = i;
            while (cp1 <= pos && pos < cp2) {
                pos = index_lookup[parent1[pos]];
            }
            
            child[pos] = parent2[i];
        }
    }

    printCrossover(parent1, parent2, child);
    checkMissingOrDuplicatedClients(cromossomeSize, child);

    std::cout << "Distance between chosen CP1 and CP2: " << abs(cp1-cp2) << " | parent1[" << cp1 << "] = " << parent1[cp1] << " | parent1[" << cp2 << "] = " << parent1[cp2] << std::endl; 
    
    return child;
}

std::vector<int> Crossover::OX(Solution *p1, Solution *p2, RandomGenerator *rng) {
    // encode parents
    // vector<int> parent1 = p1->encode();
    // vector<int> parent2 = p2->encode();

    vector<int> parent1 = p1->getGiantTour();
    vector<int> parent2 = p2->getGiantTour();

    int chromossomeSize = parent1.size();

    // generate random crossover points
    int cp2 = 0, cp1 = rng->getInt(0, chromossomeSize-1);
    
    while (cp2 == cp1 || cp2 == 0) {
        cp2 = rng->getInt(0, chromossomeSize-1);
    }

    if (cp1 > cp2) {
        int aux = cp1;
        cp1 = cp2;
        cp2 = aux;
    }

    //std::cout << "CP1: " << cp1 << " [" << parent1[cp1] << "]" << " | CP2: " << cp2 << " [" << parent1[cp2] << "]" << std::endl;

    std::vector<bool> is_direct(chromossomeSize, false);
    for (size_t idx = cp1; idx != cp2; idx++) {
        is_direct[parent1[idx]] = true;
    }

    vector<int> child = parent1;

    size_t child_pos = 0;
    for (size_t idx = 0; idx < chromossomeSize; idx++) {
        const int gene = parent2[idx];
        if (!is_direct[gene]) {
            if (child_pos == cp1) {
                child_pos = cp2; // skip [cp1, cp2)
            }
            child[child_pos++] = gene;
        }
    }

    //std::cout << "After crossover:" << std::endl;
    printCrossover(parent1, parent2, child);
    checkMissingOrDuplicatedClients(chromossomeSize, child);

    std::cout << "Distance between chosen CP1 and CP2: " << abs(cp1-cp2) << " | parent1[" << cp1 << "] = " << parent1[cp1] << " | parent1[" << cp2 << "] = " << parent1[cp2] << std::endl; 
    
    //check % of child edges similar to parent
    // if (!checkChildrenSimilarity(chromossomeSize, child, parent1)) {
    //     if (!checkChildrenSimilarity(chromossomeSize, child, parent2))
    //         return child;
    // }

    // vector<int> aux;
    // return aux;

    return child;
}