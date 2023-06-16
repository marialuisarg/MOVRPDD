#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

using namespace std;

#include <string>

class Node {
    private:
        long int ID;
        string stringID;
        char Type;
        double X;
        double Y;
        double Demand;
        int ServiceBy;  // 0 - none, 1 - D/T, 2 - T

    public:
        Node(long int ID, string stringID, char type, double x, double y, double demand, int serviceBy);
        ~Node();
        long int getID();
        char getType() const;
        double getX() const;
        double getY() const;
        double getDemand() const;
        int getServiceBy() const;
        string getStringID() const;
        void printNode();
        double euclideanDistance(Node *node);
        double manhattanDistance(Node *node);
};

#endif // NODE_H_INCLUDED