#ifndef RANDOMGENERATOR_HPP
#define RANDOMGENERATOR_HPP

#include <random>

class RandomGenerator {
private:
    std::mt19937 gen;
    unsigned int seed;

public:
    RandomGenerator(unsigned int seed) : gen(seed), seed(seed) {}

    int getInt(int min, int max) {
        std::uniform_int_distribution<> dist(min, max); // [min,max)
        return dist(gen);
    }

    double getDouble(double min, double max) {
        std::uniform_real_distribution<> dist(min, max); // [min,max)
        return dist(gen);
    }

    std::mt19937& getEngine() { return gen; }

    unsigned long long getSeed() const { return this->seed; }
};

#endif
