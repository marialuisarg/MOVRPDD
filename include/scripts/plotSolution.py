import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import pandas as pd
import random as random
import sys

def addEuclideanArrow(ax1, xInicio, yInicio, xFim, yFim, cor, legenda, lineStyle='-'):
    if legenda == "":
        ax1.arrow(xInicio, yInicio,(xFim-xInicio) , (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, linestyle=lineStyle)
    else:
        ax1.arrow(xInicio, yInicio,(xFim-xInicio) , (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, label=legenda, linestyle=lineStyle)

def addManhattanArrow(ax1, xInicio, yInicio, xFim, yFim, cor, legenda, lineStyle='-'):
    x0 = [xInicio, xFim]
    y0 = [yInicio, yInicio]
    
    if legenda == "":
        ax1.plot(x0, y0, color=cor, linestyle=lineStyle)
        ax1.arrow((xFim), (yInicio), (xFim-xFim), (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, linestyle=lineStyle)
    else:
        ax1.plot(x0, y0, color=cor, linestyle=lineStyle)
        ax1.arrow((xFim), (yInicio), (xFim-xFim), (yFim-yInicio), width = 0.001, head_width = 0.1, head_length = 0.15, length_includes_head=True, color=cor, label=legenda, linestyle=lineStyle)

def plotSolution(inst, sol):
    
    # INSTÂNCIA
    try:
        f = open(inst, "r")
    except FileNotFoundError:
        print(f"!!! ERRO: Arquivo de instância não encontrado em '{inst}'")
        return

    # lê o número de clientes contando as linhas
    for count, line in enumerate(f):
        pass
    
    numLines = count
    # Se o arquivo não tiver linha em branco no final, count pode ser o número de linhas - 1
    print(f"Arquivo de instância '{inst}' tem {count + 1} linhas no total. Serão lidos {numLines} vértices.")
    
    f.seek(0)
    f.readline() # Pula o cabeçalho
    id = []
    x = []
    y = []
    drone = []
    request = []
    
    for i in range(numLines):
        line = f.readline().split('\t\t')
        
        # Verificação de segurança para linhas mal formatadas
        if len(line) < 6:
            print(f"!!! AVISO: A linha {i+2} do arquivo de instância parece mal formatada ou está em branco: {line}")
            continue
            
        try:
            id.append(int(line[0][1:]))
            x.append(float(line[2]))
            y.append(float(line[3]))
            request.append(line[4])
            
            # .strip() remove espaços em branco e quebras de linha
            if (line[5].strip() == "T" or line[5].strip() == "-"):
                drone.append(0)
            else:
                drone.append(1)
        except (ValueError, IndexError) as e:
            print(f"!!! ERRO ao processar a linha {i+2} do arquivo de instância: {line}. Erro: {e}")
            continue

    f.close()
    
    # ---- PRINT DE VERIFICAÇÃO 1 ----
    print(f"-> Foram lidos {len(x)} pontos da instância.")
    if not x:
        print("!!! PROBLEMA: Nenhuma coordenada foi lida. Verifique o arquivo de instância e o separador ('\\t\\t').")
        return

    fig = plt.figure()
    ax1 = fig.add_subplot()
    ax = plt.gca()

    firstDrone = True
    firstTruck = True
    
    rangeX = max(x)-min(x) if x else 0
    rangeY = max(y)-min(y) if y else 0

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
        ax.annotate(id[i+1], xy=(x[i+1],y[i+1]-0.1), size=6, horizontalalignment='center', verticalalignment='top')
    
    # SOLUÇÃO
    try:
        f0 = open(sol, "r")
    except FileNotFoundError:
        print(f"!!! ERRO: Arquivo de solução não encontrado em '{sol}'")
        return
    
    # ---- PRINT DE VERIFICAÇÃO 2 ----
    header_numLines = f0.readline()
    print(f"Cabeçalho do arquivo de solução (nº de rotas): '{header_numLines.strip()}'")
    try:
        # numLines é o número de veículos/rotas
        numVehicles = int(header_numLines)
    except ValueError:
        print("!!! PROBLEMA: Não foi possível converter o número de rotas em um inteiro. Verifique a primeira linha do arquivo de solução.")
        return

    f1 = f0.readline()
    f2 = f0.readline()
    f3 = f0.readline()
    
    print(f"-> Lendo {numVehicles} rotas do arquivo de solução.")
    if numVehicles == 0:
        print("!!! AVISO: O arquivo de solução indica 0 rotas para plotar.")

    numVertex = len(x)
    
    for i in range(numVehicles):
        truck_route_line = f0.readline()
        if not truck_route_line:
            print(f"!!! AVISO: Fim inesperado do arquivo de solução ao tentar ler a rota do caminhão {i}.")
            break
        
        line = truck_route_line.strip().split("-")
        
        # ---- PRINT DE VERIFICAÇÃO 3 ----
        print(f"  - Rota de caminhão {i}: {line}")
        
        r, g, b = (random.random(), random.random(), random.random())
        color = (r, g, b)
        
        if len(line) > 1 and line[0] != '':
            for j in range(len(line)-1):
                inicio = int(line[j])
                fim = int(line[j+1])
                
                if fim==numVertex: fim=0
                if inicio==numVertex: inicio=0
                    
                if(j==0):
                    addManhattanArrow(ax1,x[inicio],y[inicio],x[fim],y[fim],color,'Truck ' + str(i))
                else:
                    addManhattanArrow(ax1,x[inicio],y[inicio],x[fim],y[fim],color,'')
        else:
            print(f"  - Rota de caminhão {i} está vazia ou mal formatada.")

        drone_flights_line = f0.readline()
        if not drone_flights_line:
            print(f"!!! AVISO: Fim inesperado do arquivo ao tentar ler os voos de drone da rota {i}.")
            break

        line_drone = drone_flights_line.strip().split("/")
        # ---- PRINT DE VERIFICAÇÃO 4 ----
        print(f"  - Voos de drone {i}: {line_drone}")
        
        for j in range(1,len(line_drone)):
            flight = line_drone[j].split("-")
            
            if len(flight) < 3:
                print(f"   !!! AVISO: Voo de drone mal formatado: {flight}")
                continue
                
            inicio = int(flight[0])
            atendido = int(flight[1])
            fim = int(flight[2])
            
            if fim==numVertex: fim=0
            if inicio==numVertex: inicio=0
            if atendido==numVertex: atendido=0
                
            if(j==1):
                addEuclideanArrow(ax1,x[inicio],y[inicio],x[atendido],y[atendido],color,'Drone ' + str(i), ':')
                addEuclideanArrow(ax1,x[atendido],y[atendido],x[fim],y[fim],color,'', ':')
            else:               
                addEuclideanArrow(ax1,x[inicio],y[inicio],x[atendido],y[atendido],color,'',':')
                addEuclideanArrow(ax1,x[atendido],y[atendido],x[fim],y[fim],color,'', ':')
    
    f0.close()
    plt.legend()
    plt.title("Energy C.: " + str(f1.strip()) + " | Delivery Cost: " + str(f2.strip()) + " | Delivery Time: " + str(f3.strip()))

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Uso: python seu_script.py <arquivo_instancia> <arquivo_solucao>")
        sys.exit(1) # Sai do script se os argumentos não forem fornecidos

    instance = sys.argv[1]
    solution = sys.argv[2]
    
    plotSolution(instance, solution)
    
    print("\nExecução finalizada. Exibindo o gráfico...")
    plt.show()