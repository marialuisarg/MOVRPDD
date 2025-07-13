import matplotlib.pyplot as plt
import pandas as pd
import random as random
import sys

def addEuclideanArrow(ax1, xInicio, yInicio, xFim, yFim, cor, legenda, lineStyle='-'):
    if legenda == "":
<<<<<<< HEAD
        ax1.arrow(xInicio, yInicio,(xFim-xInicio) , (yFim-yInicio), width = 0.001, head_width = 0.5, head_length = 0.5, length_includes_head=True, color=cor, linestyle=lineStyle)
    else:
        ax1.arrow(xInicio, yInicio,(xFim-xInicio) , (yFim-yInicio), width = 0.001, head_width = 0.5, head_length = 0.5, length_includes_head=True, color=cor, label=legenda, linestyle=lineStyle)
=======
        ax1.arrow(xInicio, yInicio,(xFim-xInicio) , (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, linestyle=lineStyle)
    else:
        ax1.arrow(xInicio, yInicio,(xFim-xInicio) , (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, label=legenda, linestyle=lineStyle)
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)
        
def addManhattanArrow(ax1, xInicio, yInicio, xFim, yFim, cor, legenda, lineStyle='-'):
    x0 = [xInicio, xFim]
    y0 = [yInicio, yInicio]
    
    if legenda == "":
        ax1.plot(x0, y0, color=cor, linestyle=lineStyle)
<<<<<<< HEAD
        ax1.arrow((xFim), (yInicio), (xFim-xFim), (yFim-yInicio), width = 0.001, head_width = 0.5, head_length = 0.5, length_includes_head=True, color=cor, linestyle=lineStyle)
    else:
        ax1.plot(x0, y0, color=cor, linestyle=lineStyle)
        ax1.arrow((xFim), (yInicio), (xFim-xFim), (yFim-yInicio), width = 0.001, head_width = 0.5, head_length = 0.5, length_includes_head=True, color=cor, label=legenda, linestyle=lineStyle)
=======
        ax1.arrow((xFim), (yInicio), (xFim-xFim), (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, linestyle=lineStyle)
    else:
        ax1.plot(x0, y0, color=cor, linestyle=lineStyle)
        ax1.arrow((xFim), (yInicio), (xFim-xFim), (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, label=legenda, linestyle=lineStyle)
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)

def plotSolution(inst, sol):
    
    # INSTÂNCIA
    f = open(inst, "r")
    
    # lê o número de clientes contando as linhas
    for count, line in enumerate(f):
        pass
    
    numLines = count
    numVertex = numLines
    
    f.seek(0)
    f.readline()
    id = []
    x = []
    y = []
    drone = []
    request = []
    
    for i in range(numLines):
<<<<<<< HEAD
        line = f.readline().split('\t')
        id.append(int(line[0]))
=======
        line = f.readline().split('\t\t')
        # print(line)
        id.append(int(line[0][1:]))
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)
        x.append(float(line[2]))
        y.append(float(line[3]))
        request.append(line[4])
        # print(i)
<<<<<<< HEAD
        # print(line[5])
        if (line[5] == "T\n" or line[5] == "T"):
=======
        if (line[5] == "T\n" or line[5] == "T" or line[5] == "-\n"):
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)
            drone.append(0)
        else:
            drone.append(1)
    f.close()
    
    fig = plt.figure()
    ax1 = fig.add_subplot()
    ax = plt.gca()

    firstDrone = True
    firstTruck = True
    
    rangeX = max(x)-min(x)
    rangeY = max(y)-min(y)

    if(rangeX > rangeY):
        space = rangeX - rangeY
        plt.xlim([min(x)-5, max(x)+5])
        plt.ylim([min(y)-space/2 - 5, max(y)+space/2 + 5])
    else:
        space = rangeY - rangeX
        plt.ylim([min(y)-5, max(y)+5])
        plt.xlim([min(x)-space/2 - 5, max(x)+space/2 + 5])
    
    plt.plot(x[0], y[0], 'gs', label='Depot')
    
    for i in range(len(x)-1):
        if (drone[i+1]==1):
            if(firstDrone):
                plt.plot(x[i+1],y[i+1],'o', color='aqua', label='Truck/drone clients')
                firstDrone = False
            else:
                plt.plot(x[i+1],y[i+1],'o', color='aqua')
        else:
            if(firstTruck):
                plt.plot(x[i+1],y[i+1],'o', color='lime', label='Truck clients')
                firstTruck = False
            else:
                plt.plot(x[i+1],y[i+1],'o', color='lime')
    for i in range(len(x)-1):
        ax.annotate(xy=(x[i+1],y[i+1]+0.1), size=6, horizontalalignment='center', verticalalignment='bottom', text="")
        
        # nomeando os clientes por request
        # ax.annotate(request[i+1], xy=(x[i+1],y[i+1]-0.1), size=6, horizontalalignment='center', verticalalignment='top')
        
        # nomeando os clientes por id
        ax.annotate(id[i+1], xy=(x[i+1],y[i+1]-0.1), size=6, horizontalalignment='center', verticalalignment='top')
    
    
    # SOLUÇÃO
    
    f0 = open(sol, "r")
    
    # truck solution
    numLines = int(f0.readline())
    f1 = f0.readline()
    f2 = f0.readline()
    f3 = f0.readline()
    
    for i in range(numLines):
        line = f0.readline()[:-1].split("-")
        
        r = random.random()*i%1 
        g = random.random()*i%1
        b = random.random()*i%1
        
        color = (r,g,b)
        
        for j in range(len(line)-1):
            inicio = int(line[j])
            fim = int(line[j+1])
            
            if fim==numVertex:
                fim=0
                
            if inicio==numVertex:
                inicio=0
                
            if(j==0):
                addManhattanArrow(ax1,x[inicio],y[inicio],x[fim],y[fim],color,'Truck ' + str(i))
            else:
                addManhattanArrow(ax1,x[inicio],y[inicio],x[fim],y[fim],color,'')
        
        line = f0.readline()[:-1].split("/")
        for j in range(1,len(line)):
            flight = line[j].split("-")
            
            inicio = int(flight[0])
            atendido = int(flight[1])
            fim = int(flight[2])
            
            if fim==numVertex:
                fim=0
            if inicio==numVertex:
                inicio=0
            if atendido==numVertex:
                atendido=0
                
            if(j==1):
                addEuclideanArrow(ax1,x[inicio],y[inicio],x[atendido],y[atendido],color,'Drone ' + str(i), ':')
                addEuclideanArrow(ax1,x[atendido],y[atendido],x[fim],y[fim],color,'', ':')
            else:                
                addEuclideanArrow(ax1,x[inicio],y[inicio],x[atendido],y[atendido],color,'',':')
                addEuclideanArrow(ax1,x[atendido],y[atendido],x[fim],y[fim],color,'', ':')
    plt.legend()
    plt.title("Energy C.: " + str(f1) + " | Delivery Cost: " + str(f2) + " | Delivery Time: " + str(f3))

if __name__ == '__main__':
    instance = sys.argv[1]
<<<<<<< HEAD
    tSolution = sys.argv[2]
    solution = sys.argv[3]
    
    # truck solution
    plotSolution(instance, tSolution)
=======
    #tSolution = sys.argv[2]
    solution = sys.argv[2]
    
    # # truck solution
    # plotSolution(instance, tSolution)
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)
    
    # truck/drone solution
    plotSolution(instance, solution)
    plt.show()