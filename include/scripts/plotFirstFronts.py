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
        norm = (functions - min) / (max - min)

        norm_func.append(norm)

    return norm_func

def plot_graph(filename):
    norm_func = read_objectives(filename)
    colors = plt.cm.viridis(np.linspace(0, 1, len(norm_func)))  # Generate a color map

    # Plotagem dos gráficos
    fig, axs = plt.subplots(1, 2, figsize=(14, 6))


    for i, front in enumerate(norm_func):
        # if ((i+1)%10 == 0):
        # Plot Objetivo 1 vs Objetivo 2
        axs[0].scatter(front[:, 1], front[:, 0], c=[colors[i]], alpha=0.4, s=10)

        # Plot Objetivo 1 vs Objetivo 3
        axs[1].scatter(front[:, 1], front[:, 2], c=[colors[i]], label=f'Gen {i+1}', alpha=0.4, s=10)

            
    # Configurações do gráfico f1 vs f2
    axs[0].set_xlabel('f2')
    axs[0].set_ylabel('f1')
    axs[0].set_title('f1 vs f2')

    # Configurações do gráfico f2 vs f3
    axs[1].set_xlabel('f2')
    axs[1].set_ylabel('f3')
    axs[1].set_title('f3 vs f2')
    axs[1].legend()

    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    filename = sys.argv[1]
    plot_graph(filename)
