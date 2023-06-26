import pandas as pd
import sys

from tabulate import tabulate

def printTable(inst, func):
    f0 = open(func, "r")
    
    numSol = int(f0.readline())
    
    table = [["Solution", "Energy Comsumption", "Delivery Cost", "Delivery Time"]]
    
    smF1 = [float("inf"), 0, 1]
    smF2 = [float("inf"), 0, 2]
    smF3 = [float("inf"), 0, 3]
    
    for i in range(numSol):
        line = f0.readline().split(" ")
        f1 = line[0]
        if (float(f1) < smF1[0]):
            smF1[0] = float(f1)
            smF1[1] = i
        f2 = line[1]
        if (float(f2) < smF2[0]):
            smF2[0] = float(f2)
            smF2[1] = i
        f3 = line[2]
        if (float(f3) < smF3[0]):
            smF3[0] = float(f3)
            smF3[1] = i
        table += [[i, f1, f2, f3]]
    
    print("Instance: " + inst)
    print(tabulate(table, headers="firstrow", tablefmt="fancy_grid"))
    
    
if __name__ == '__main__':
    instance = sys.argv[1]
    functions = sys.argv[2]
    
    printTable(instance, functions)