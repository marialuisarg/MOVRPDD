import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from matplotlib.patches import FancyArrowPatch
import numpy as np
import sys

def readSolution(inst, sol, G):
    
    # INSTÂNCIA
    f = open(inst, "r")
    
    # lê o número de clientes contando as linhas
    for count, line in enumerate(f):
        pass
    
    numLines = count
    numVertex = numLines
    
    f.seek(0)
    f.readline()
    
    pos = {}
    
    for i in range(numLines):
        line = f.readline().split('\t')
        pos[i] = (float(line[4]), float(line[6]))
        
    f.close()
    
    # SOLUÇÃO
    f0 = open(sol, "r")
    
    numFronts = int(f0.readline())
    f0.readline()
    
    for i in range(numFronts):
        f0.readline()
        numSolutions = int(f0.readline())
        for j in range(numSolutions):
            numRoutes = int(f0.readline())
            f0.readline()
            for k in range(numRoutes):
                line = f0.readline()[:-1].split("-")
                for l in range(len(line)-1):
                    inicio = int(line[l])
                    fim = int(line[l+1])
                    
                    if fim==numVertex:
                        fim=0
                        
                    if inicio==numVertex:
                        inicio=0
                        
                    if G.has_edge(inicio, fim) and G[inicio][fim]['type'] == 'truck':
                        G[inicio][fim]['weight'] += 1
                    else:
                        G.add_edge(inicio, fim, weight=1, type='truck')
                        
                line = f0.readline()[:-1].split("/")
                for l in range(1,len(line)):
                    flight = line[l].split("-")
                    
                    inicio = int(flight[0])
                    atendido = int(flight[1])
                    fim = int(flight[2])
                    
                    if fim==numVertex:
                        fim=0
                    if inicio==numVertex:
                        inicio=0
                    if atendido==numVertex:
                        atendido=0
                        
                    if G.has_edge(inicio, atendido) and G[inicio][atendido]['type'] == 'drone':
                        G[inicio][atendido]['weight'] += 1
                    else:
                        G.add_edge(inicio, atendido, weight=1, type='drone')
                    
                    if G.has_edge(atendido, fim) and G[atendido][fim]['type'] == 'drone':
                        G[atendido][fim]['weight'] += 1
                    else:
                        G.add_edge(atendido, fim, weight=1, type='drone', style='--',connectionstyle=f'arc3,rad={0.2}')
                        
                    # G.nodes[atendido]['drone'] += 1
        f0.readline()
        
    return pos
                        
                    
if __name__ == '__main__':
    instance = sys.argv[1]
    solution = sys.argv[2]
    
    G = nx.DiGraph()
    
    pos = readSolution(instance, solution, G)
    
    # Normaliza pesos para determinar a cor e a espessura das arestas
    max_weight = max(nx.get_edge_attributes(G, 'weight').values())
    edges = G.edges(data=True)
   
    weights = np.array([G[u][v]['weight'] for u, v in G.edges()])
    norm = mcolors.Normalize(vmin=min(weights), vmax=max(weights))
    cmap = plt.get_cmap('viridis')  # Você pode escolher outro cmap se desejar
    
    widths = [1 + 4 * (data['weight'] / max_weight) for _, _, data in edges]

    # Criar a figura e os eixos
    fig, ax = plt.subplots(figsize=(10, 6))

    #region Grafo completo
    # Desenha o grafo
    nx.draw(G, pos, with_labels=True, node_color='lightblue', edge_color=weights, edge_cmap=cmap, width=1, node_size=350, font_size=10)
    #endregion
    
    # Mapa de cores
    sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)    
    sm.set_array([])
    plt.colorbar(sm, ax=ax)
    
    #region Grafo filtrado por peso
    # # Filtrar as arestas com peso maior que x
    # filtered_edges = [(u, v) for u, v, weight in G.edges(data='weight') if weight > 5]

    # # Criar um novo grafo apenas com as arestas filtradas
    # H = G.edge_subgraph(filtered_edges)

    # # Definir o layout do grafo
    # pos = nx.spring_layout(G)

    # # Plotar o grafo com as arestas filtradas
    # plt.figure(figsize=(8, 6))
    # nx.draw(H, pos, with_labels=True, node_color='lightblue', edge_color='grey', width=1, style='-')
    #endregion 
    
    filename = solution[41:-4]
    filename = './solutions/graphs/' + filename + '.png'
    print(filename)
    plt.savefig(filename, format='png')

    # Exibe o grafo
    #plt.show()
