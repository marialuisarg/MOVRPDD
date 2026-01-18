import matplotlib.pyplot as plt
import numpy as np
from sklearn.preprocessing import MinMaxScaler
import sys
import os

def read_data(filename, delimiter=' '):
    data = np.loadtxt(filename, delimiter=delimiter)
    return data

def normalize_data(data):
    scaler = MinMaxScaler()
    normalized_data = scaler.fit_transform(data)
    return normalized_data

def load_all_data(directory, delimiter=' '):
    all_data = []
    file_names = sorted(os.listdir(directory))  # Sort to maintain order of generations
    for file_name in file_names:
        file_path = os.path.join(directory, file_name)
        data = read_data(file_path, delimiter)
        normalized_data = normalize_data(data)
        all_data.append(normalized_data)
    return all_data

def plot_pareto_frontiers(all_data):
    colors = plt.cm.viridis(np.linspace(0, 1, len(all_data)))  # Generate a color map

    fig, axs = plt.subplots(1, 2, figsize=(12, 6))
    
    for idx, data in enumerate(all_data):
        # Plot Objetivo 1 vs Objetivo 2
        axs[0].scatter(data[:, 0], data[:, 1], c=[colors[idx]], label=f'Generation {idx+1}', alpha=0.6, s=10)
        # Plot Objetivo 1 vs Objetivo 3
        axs[1].scatter(data[:, 0], data[:, 2], c=[colors[idx]], label=f'Generation {idx+1}', alpha=0.6, s=10)


    axs[0].set_xlabel('F1')
    axs[0].set_ylabel('F2')
    axs[0].set_title('F1 vs F2')
    #axs[0].legend()
    axs[0].grid(True)

    axs[1].set_xlabel('F1')
    axs[1].set_ylabel('F3')
    axs[1].set_title('F1 vs F3')
    #axs[1].legend()
    axs[1].grid(True)

    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    # filename = sys.argv[1]
    directory = './solutions/generations/by_objectives/'
    #num_lines = count_lines_in_file(filename)
    
    # Carregar os dados de todas as gerações
    all_data = load_all_data(directory)

    # Plotar as fronteiras de Pareto para todas as gerações
    plot_pareto_frontiers(all_data)
