#ifndef NODE_H_
#define NODE_H_

using namespace std;

#include <string>
#include "Types.hpp"

class Node {
    private:
        long int    ID;
        string      stringID;
        char        Type;
        double      X;
        double      Y;
        double      Demand;
        int         ServiceBy;

    public:
        Node (long int clientID, string clientStringID, char type, double x, double y, double demand, int serviceBy) {
            ID          = clientID;
            stringID    = clientStringID;
            Type        = type;
            X           = x;
            Y           = y;
            Demand      = demand;
            ServiceBy   = serviceBy;
        };

        ~Node(){};

        long int getID() {  return this->ID;  };
        
        char getType() const {  return this->Type;  };
        
        double getX() const {  return this->X;  };
        
        double getY() const {  return this->Y;  }
        
        double getDemand() const {  return this->Demand;  }
        
        int getServiceBy() const {  return this->ServiceBy;  }
        
        string getStringID() const {  return this->stringID;  }
        
        double euclideanDistance(Node *node) {
            if (getX() == node->getX() && getY() == node->getY())
                return 0;
            
            return sqrt(pow((getX() - node->getX()), 2) + pow((getY() - node->getY()), 2));
        };
        
        double manhattanDistance(Node *node) {
            if (getX() == node->getX() && getY() == node->getY())
                return 0;
            return abs(getX() - node->getX()) + abs(getY() - node->getY());
        };
        
        void printNode() {
            std::cout << stringID << "\t\t";
            std::cout << ID << "\t";
            std::cout << Type << "\t";
            std::cout << X << "\t";
            std::cout << Y << "\t";
            std::cout << Demand << "\t";
            
            switch (ServiceBy) {
                case DEPOT:
                    std::cout << "-";
                    break;
                case TRUCK_DRONE:
                    std::cout << "D/T";
                    break;
                case TRUCK:
                    std::cout << "T";
                    break;
                default:
                    std::cout << "Error";
                    break;
            }

            std::cout << std::endl;
        };
        
};

#endif // NODE_H_INCLUDED