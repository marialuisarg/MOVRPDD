import sys
import plotly.express as px
import numpy as np
import pandas as pd


def read_objectives(filename):
    with open(filename, 'r') as f:
        all_data = f.read().strip()

    generations = all_data.split('\n\n')
    func = []

    for geracao in generations:
        # Separar as funções objetivo da geração
        functions = np.array([list(map(float, linha.split())) for linha in geracao.splitlines()])

        # # Normalização: (valor - min) / (max - min) por coluna (f1, f2, f3)
        # min_vals = np.min(functions, axis=0)
        # max_vals = np.max(functions, axis=0)
        # ranges = max_vals - min_vals
        # ranges[ranges == 0] = 1  # Evita divisão por zero
        # norm = (functions - min_vals) / ranges

        func.append(functions)

    return func

def plot_radar_chart(filename):
    
    func = read_objectives(filename)
    
    all_solutions = np.vstack([gen for gen in func if gen.size > 0])
    
    min_vals = np.min(all_solutions, axis=0)
    max_vals = np.max(all_solutions, axis=0)
    ranges = max_vals - min_vals
    
    # Evita divisão por zero se um objetivo tiver o mesmo valor sempre
    ranges[ranges == 0] = 1 

    normalized_func = [(generation_data - min_vals) / ranges for generation_data in func]
    # --- FIM DA ETAPA DE NORMALIZAÇÃO ---


    # Calcula a média de cada objetivo (F1, F2, F3) para cada geração
    # AGORA USANDO OS DADOS NORMALIZADOS
    avg_objectives = []
    for i, generation_data in enumerate(normalized_func):
        if generation_data.size > 0:
            # np.mean(axis=0) calcula a média de cada coluna (F1, F2, F3)
            means = np.mean(generation_data, axis=0)
            if len(means) == 3:
                avg_objectives.append({
                    'Geracao': i + 1,
                    'F1': means[0],
                    'F2': means[1],
                    'F3': means[2]
                })

    if not avg_objectives:
        print("Não foi possível calcular as médias dos objetivos.")
        return

    # Cria um DataFrame do pandas para facilitar a manipulação e a plotagem
    df = pd.DataFrame(avg_objectives)

    # Filtra o DataFrame para incluir apenas as gerações múltiplas de 100
    # A geração 1 também é incluída para ter um ponto de partida
    df_filtered = df[(df['Geracao'] == 75) | (df['Geracao'] == 10) | (df['Geracao'] == 20) | (df['Geracao'] % 50 == 0)]

    # Prepara os dados para o formato que o plotly espera ("long format")
    df_melted = df_filtered.melt(id_vars=['Geracao'], value_vars=['F1', 'F2', 'F3'],
                                 var_name='Objetivo', value_name='Valor Médio Normalizado')

    # Cria o gráfico de radar (spider chart)
    fig = px.line_polar(df_melted,
                        r='Valor Médio Normalizado',
                        theta='Objetivo',
                        color='Geracao',
                        line_close=True,
                        title='Evolução das Médias Normalizadas dos Objetivos por Geração',
                        markers=True,
                        labels={"Geracao": "Geração"},
                        range_r=[0,1]) # Define o eixo do raio de 0 a 1

    # Salva o gráfico em um arquivo HTML interativo
    fig.write_html("radar_chart_evolucao_normalizado.html")
    print("Gráfico de radar normalizado salvo como 'radar_chart_evolucao_normalizado.html'")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Uso: python seu_script.py <nome_do_arquivo_de_dados>")
        sys.exit(1)
    else:
        filename = sys.argv[1]

    plot_radar_chart(filename)
