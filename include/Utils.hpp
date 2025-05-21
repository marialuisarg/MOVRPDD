#ifndef UTILS_H_
#define UTILS_H_

#include<fstream>
#include<iostream>
#include<sstream>

#include<vector>
#include<tuple>
#include<string.h>
#include<cmath>

#include "Types.hpp"
#include "Graph.hpp"
#include "Node.hpp"
#include "Solution.hpp"

namespace Util{
    
    static vector<string> split(string str, char delimiter) {
        vector<string> s;
        stringstream ss(str);
        string part;
        while(getline(ss, part, delimiter))
            s.push_back(part);
        return s;
    };

    static int defineServiceBy(string ServiceByString) {
        int ServiceBy;

        ServiceByString.pop_back();

        if (strcmp(ServiceByString.c_str(), "-") == 0){
            ServiceBy = DEPOT;
        } else if (strcmp(ServiceByString.c_str(), "D/T") == 0){
            ServiceBy = TRUCK_DRONE;
        } else {
            ServiceBy = TRUCK;
        }

        return ServiceBy;
    };

    static vector<Node> readFile(string fileName) {
        int ID;
        string stringID;
        char Type;
        double X;
        double Y;
        double Demand;
        int ServiceBy;
        string ServiceByString;
        fileName = "./" + fileName;
        string line;
        
        ifstream arq(fileName);
        vector <string> s;
        vector<string> lineBroken;

        vector<Node> nodes;

        if(arq.is_open()){
            cout << "Arquivo aberto com sucesso!" << std::endl;
            getline(arq,line);
            string delimiter = "\t\t";
            int cont = 0;
                
            while(getline(arq,line)){
                size_t pos = 0;
                string token;

                while ((pos = line.find(delimiter)) != string::npos) {
                    token = line.substr(0, pos);
                    lineBroken.push_back(token);
                    line.erase(0, pos + delimiter.length());
                }
                lineBroken.push_back(line);

                stringID = lineBroken[0];
                ID = cont;
                cont++;
                
                Type = lineBroken[1].c_str()[0];
                X = stod(lineBroken[2]);
                Y = stod(lineBroken[3]);
                Demand = stod(lineBroken[4]);
                ServiceByString = lineBroken[5];
                ServiceBy = defineServiceBy(ServiceByString);
                Node aux = Node(ID, stringID, Type, X, Y, Demand, ServiceBy);
                nodes.push_back(aux);
                lineBroken.clear();
            }

            arq.close();
        }else{
            cout << "Erro ao abrir o arquivo!" << std::endl;
            exit(0);
        }

        return nodes;
    };
    
    static int getRandomInteger(int begin, int end) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(begin, end);
        return dis(gen);
    };

    static void printSolutionsToFile(vector<Solution*> s, string instanceName) {
        string filename = "./solutions/by_objectives/functions_" + instanceName.erase(0,12);
    
        ofstream output_file(filename);

        int nSolutions = s.size();

        double f1 = 0.0, f2 = 0.0, f3 = 0.0;

        output_file << nSolutions << endl;

        for (int i = 0; i < nSolutions; i++) {
            f1 = s.at(i)->getTotalEnergyConsumption();
            f2 = s.at(i)->getTotalDeliveryCost();
            f3 = s.at(i)->getTotalDeliveryTime();

            output_file << f1 << " " << f2 << " " << f3 << std::endl;
        }

        output_file.close();
        
        string command = "python ./scripts/printFuncToTable.py " + instanceName + " " + filename;
        int aux = system(command.c_str());
    };
    
    static void printSolutionsToFile(vector<Solution*> s, string instanceName, string setName, bool normalized) {
        string filename = "./solutions/by_objectives/functions_" + setName + "_" + instanceName.erase(0,12);
    
        ofstream output_file(filename);

        int nSolutions = s.size();

        double f1 = 0.0, f2 = 0.0, f3 = 0.0;
        double max_f1 = 0.0, max_f2 = 0.0, max_f3 = 0.0;

        output_file << nSolutions << endl;

        for (int i = 0; i < nSolutions; i++) {
            f1 = s.at(i)->getTotalEnergyConsumption();
            f2 = s.at(i)->getTotalDeliveryCost();
            f3 = s.at(i)->getTotalDeliveryTime();

            output_file << f1 << " " << f2 << " " << f3 << std::endl;
        }

        output_file.close();
        
        string command = "python ./scripts/printFuncToTable.py " + instanceName + " " + filename + " " + (normalized ? "True" : "False");
        int aux = system(command.c_str());
    };
    
    static void printFunctionsByGenerationToFile(vector<vector<Solution*>> s, string instanceName, string setName, bool normalized) {
        string filename = "./solutions/generations/by_objectives/" + setName + "_" + instanceName.erase(0,12);
    
        ofstream output_file(filename);

        int nFronts = s.size();
        
        double f1 = 0.0, f2 = 0.0, f3 = 0.0;

        for (int i = 0; i < nFronts; i++) {
            for (int j = 0; j < s.at(i).size(); j++) {
                f1 = s.at(i).at(j)->getTotalEnergyConsumption();
                f2 = s.at(i).at(j)->getTotalDeliveryCost();
                f3 = s.at(i).at(j)->getTotalDeliveryTime();

                output_file << s.at(i).at(j)->getNumRoutes();
                output_file << f1 << " " << f2 << " " << f3 << std::endl;
                
                // print route to plot later
                //s.at(i).at(j)->saveRouteToPlot(output_file);
                //output_file << std::endl;
                
                // string plotFile = "./solutions/generations/plots/" + setName + "_" + instanceName.erase(0,12);
                // s.at(i).at(j)->plotSolution(instanceName, j, plotFile);
            }
            //output_file << std::endl;
        }

        std::cout << "File " << filename << " created." << std::endl;
        output_file.close();

        string command = "python ./include/scripts/plotParetoFrontier.py " + filename;
        int aux = system(command.c_str());
    }

    static void printGenerationToFile(vector<vector<Solution*>> s, string instanceName, string setName, bool normalized) {
        string filename = "./solutions/generations/by_routes/routes_" + setName + "_" + instanceName.erase(0,12);
    
        ofstream output_file(filename);

        int nFronts = s.size();
        
        double f1 = 0.0, f2 = 0.0, f3 = 0.0;
        double max_f1 = 0.0, max_f2 = 0.0, max_f3 = 0.0;

        output_file << nFronts << std::endl << std::endl;

        for (int i = 0; i < nFronts; i++) {
            output_file << "Front " << i << std::endl;
            output_file << s.at(i).size() << std::endl;
            for (int j = 0; j < s.at(i).size(); j++) {

                f1 = s.at(i).at(j)->getTotalDeliveryCost();
                f2 = s.at(i).at(j)->getTotalDeliveryTime();
                f3 = s.at(i).at(j)->getTotalEnergyConsumption();

                output_file << s.at(i).at(j)->getNumRoutes();
                output_file << std::endl << f1 << " " << f2 << " " << f3 << std::endl;
                
                // print route to plot later
                s.at(i).at(j)->saveRouteToPlot(output_file);
                
                // string plotFile = "./solutions/generations/plots/" + setName + "_" + instanceName.erase(0,12);
                // s.at(i).at(j)->plotSolution(instanceName, j, plotFile);
            }
            output_file << std::endl;
        }

        std::cout << "File " << filename << " created." << std::endl;
        output_file.close();
        
        string command = "python ./include/scripts/plotNetworkGraph.py ./instances/" + instanceName + " " + filename;
        int aux = system(command.c_str());
    };

    static void plotParetoFrontiers() {
        std::cout << "Plotting Pareto Frontiers" << std::endl;
        string command = "python ./scripts/plotParetoFrontiers.py";
        int aux = system(command.c_str());
    }; 

    static void printFirstFrontsToFile(vector<vector<Solution*>> s, string instanceName, string setName) {
        string filename = "./solutions/generations/firstFront_" + instanceName.erase(0,12);
        std::ofstream output_file;

        output_file.open(filename, std::ios::app);
        
        float f1, f2, f3;

        if (output_file.is_open()) {
            for (int j = 0; j < s.at(0).size(); j++) {

                f1 = s.at(0).at(j)->getTotalDeliveryCost();
                f2 = s.at(0).at(j)->getTotalDeliveryTime();
                f3 = s.at(0).at(j)->getTotalEnergyConsumption();
                
                output_file << std::endl << f1 << " " << f2 << " " << f3;
            }
            output_file << std::endl;
            output_file.close();
        } else {
            std::cerr << "Error while trying to open file." << std::endl;
        }
        
    };
};

#endif /* UTILS_HPP_ */