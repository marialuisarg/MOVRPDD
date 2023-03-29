#include <fstream>
#include <iostream>

#include <vector>
#include <string>
#include <cmath>

#include "Utils.h"

using namespace std;

vector<string> Utils::split(string str, char delimiter){
    vector<string> s;
    stringstream ss(str);
    string part;
    while(getline(ss, part, delimiter)) {
        s.push_back(part);
    }
    return s;
}

int Utils::defineServiceBy(string ServiceByString){
    int ServiceBy;

    if(strcmp(ServiceByString.c_str(), "-") == 0){
        ServiceBy = 0;
    }else if(strcmp(ServiceByString.c_str(), "D/T") == 0){
        ServiceBy = 1;
    }else if(strcmp(ServiceByString.c_str(), "T") == 0){
        ServiceBy = 2;
    }else{
        ServiceBy = 3;
    }
    return ServiceBy;
}

vector<Node> Utils::readFile(string fileName){
    int ID;
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
        cout << "Arquivo aberto com sucesso!" << endl;
        getline(arq,line);
        	
		while(getline(arq,line)){
            lineBroken = split(line, '\t');
            ID = stoi(lineBroken[0]);
            Type = lineBroken[1].c_str()[0];
            X = stod(lineBroken[2]);
            Y = stod(lineBroken[3]);
            Demand = stod(lineBroken[4]);
            ServiceByString = lineBroken[5];
            ServiceBy = defineServiceBy(ServiceByString);
            Node aux = Node(ID, Type, X, Y, Demand, ServiceBy);
            nodes.push_back(aux);
        }

        arq.close();
    }else{
        cout << "Erro ao abrir o arquivo!" << endl;
        exit(0);
    }

    return nodes;
}