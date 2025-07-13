import sys
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D  # Import necessário para 3D

def read_objectives(filename):
    with open(filename, 'r') as f:
        all_data = f.read().strip()

    generations = all_data.split('\n\n')
    norm_func = []

    for geracao in generations:
        # Separar as funções objetivo da geração
        functions = np.array([list(map(float, linha.split())) for linha in geracao.splitlines()])

        # Normalização: (valor - min) / (max - min) por coluna (f1, f2, f3)
        min_vals = np.min(functions, axis=0)
        max_vals = np.max(functions, axis=0)
        ranges = max_vals - min_vals
        ranges[ranges == 0] = 1  # Evita divisão por zero
        norm = (functions - min_vals) / ranges

        norm_func.append(norm)

    return norm_func

def plot_3d_graph(filename):
    norm_func = read_objectives(filename)
    colors = plt.cm.inferno(np.linspace(0.1, 0.6, len(norm_func)))

    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')

    for i, front in enumerate(norm_func):
        # Plot apenas uma geração específica 
        if (i + 1) == 1:
            ax.scatter(front[:, 0], front[:, 1], front[:, 2], c=[colors[i]], label=f'Geração {i+1}', alpha=0.8, s=20)

    ax.set_xlabel('f1 (eixo X)')
    ax.set_ylabel('f2 (eixo Y)')
    ax.set_zlabel('f3 (eixo Z)')
    ax.set_title('Fronteira em 3D: f1 vs f2 vs f3')

    ax.legend()
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    filename = sys.argv[1]
    plot_3d_graph(filename)
