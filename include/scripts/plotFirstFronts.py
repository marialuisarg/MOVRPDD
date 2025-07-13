import sys
import matplotlib.pyplot as plt
import numpy as np

def read_objectives(filename):
    with open(filename, 'r') as f:
        all_data = f.read().strip()

    generations = all_data.split('\n\n')

    norm_func = []

    for geracao in generations:
        # Separar as funções objetivo da geração
        functions = np.array([list(map(float, linha.split())) for linha in geracao.splitlines()])

        # Normalização: (valor - min) / (max - min) por coluna (f1, f2, f3)
        min = np.min(functions, axis=0)
        max = np.max(functions, axis=0)
<<<<<<< HEAD
        norm = (functions - min) / (max - min)
=======
        ranges = max - min
        ranges[ranges == 0] = 1  # Evita divisão por zero
        norm = (functions - min) / ranges
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)

        norm_func.append(norm)

    return norm_func

def plot_graph(filename):
    norm_func = read_objectives(filename)
<<<<<<< HEAD
    colors = plt.cm.viridis(np.linspace(0, 1, len(norm_func)))  # Generate a color map
=======
    colors = plt.cm.inferno(np.linspace(0.1, 0.6, len(norm_func)))  # Generate a color map
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)

    # Plotagem dos gráficos
    fig, axs = plt.subplots(1, 2, figsize=(14, 6))


    for i, front in enumerate(norm_func):
<<<<<<< HEAD
        if ((i+1)%50 == 0):
            # Plot Objetivo 1 vs Objetivo 2
            axs[0].scatter(front[:, 0], front[:, 1], c=[colors[i]], alpha=0.4, s=10)

            # Plot Objetivo 1 vs Objetivo 3
            axs[1].scatter(front[:, 0], front[:, 2], c=[colors[i]], label=f'Gen {i+1}', alpha=0.4, s=10)
=======
        #if (((i+1)>=200 and (i+1)%20 == 0) or ((i+1)<200 and (i+1)%100 == 0)):
        if (((i+1) == 100)):
            # Plot Objetivo 1 vs Objetivo 2
            axs[0].scatter(front[:, 0], front[:, 1], c=[colors[i]], alpha=0.8, s=10)

            # Plot Objetivo 1 vs Objetivo 3
            axs[1].scatter(front[:, 0], front[:, 2], c=[colors[i]], label=f'Gen {i+1}', alpha=0.8, s=10)
>>>>>>> 52cc473 (fix: drone route construction algorithm | delete: big files)

            
    # Configurações do gráfico f1 vs f2
    axs[0].set_xlabel('f1')
    axs[0].set_ylabel('f2')
    axs[0].set_title('f1 vs f2')

    # Configurações do gráfico f2 vs f3
    axs[1].set_xlabel('f1')
    axs[1].set_ylabel('f3')
    axs[1].set_title('f1 vs f3')
    axs[1].legend()

    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    filename = sys.argv[1]
    plot_graph(filename)
