#ifndef GRASP_H_INCLUDED
#define GRASP_H_INCLUDED

using namespace std;

#include "Solution.h"

class GRASP {
    private:
        double alpha;
        int numIterations;
        Solution *bestSolution;
        Solution *currentSolution;

    public:
        GRASP(Graph *g, int QT, double alpha, int numIterations);
        ~GRASP();
};

#endif // GRASP_H_INCLUDED