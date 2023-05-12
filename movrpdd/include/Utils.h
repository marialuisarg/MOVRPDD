#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include<iostream>

#include<vector>
#include<string>
#include<string.h>
#include<sstream>
#include<cmath>
#include<fstream>

#include "Node.h"
#include "Solution.h"

using namespace std;

class Utils{
    public:
        Utils(){};
        ~Utils(){};
        static vector<string> split(string str, char delimiter);
        static int defineServiceBy(string ServiceByString);
        static vector<Node> readFile(string fileName);
        static void printSolution(Solution *s);
};

#endif