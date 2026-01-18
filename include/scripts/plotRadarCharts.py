import sys
import plotly.express as px
import numpy as np
import pandas as pd


def read_objectives(filename):
    """Lê os dados de objetivos de um arquivo."""
    try:
        with open(filename, 'r') as f:
            all_data = f.read().strip()
    except FileNotFoundError:
        print(f"Erro: Arquivo '{filename}' não encontrado.")
        sys.exit(1)

    generations = all_data.split('\n\n')
    func = []

    for geracao in generations:
        if geracao: # Evita processar seções vazias
            try:
                # Separar as funções objetivo da geração
                functions = np.array([list(map(float, linha.split())) for linha in geracao.splitlines()])
                func.append(functions)
            except ValueError:
                print(f"Aviso: Não foi possível converter uma linha para números no arquivo '{filename}'. Pulando a linha.")
                continue


    return func

def plot_radar_chart(filename1, filename2, generation_to_plot):
    """
    Plota um gráfico de radar comparando dados de duas fontes para uma geração específica.
    """
    # Lê os dados de ambos os arquivos
    func1 = read_objectives(filename1)
    func2 = read_objectives(filename2)

    # Combina todos os dados para uma normalização global
    all_solutions = np.vstack(
        [gen for gen in func1 if gen.size > 0] +
        [gen for gen in func2 if gen.size > 0]
    )

    if all_solutions.size == 0:
        print("Nenhum dado válido encontrado em ambos os arquivos.")
        return

    # --- ETAPA DE NORMALIZAÇÃO GLOBAL ---
    min_vals = np.min(all_solutions, axis=0)
    max_vals = np.max(all_solutions, axis=0)
    ranges = max_vals - min_vals

    # Evita divisão por zero se um objetivo tiver o mesmo valor sempre
    ranges[ranges == 0] = 1

    # Normaliza cada conjunto de dados usando os valores min/max globais
    normalized_func1 = [(generation_data - min_vals) / ranges for generation_data in func1]
    normalized_func2 = [(generation_data - min_vals) / ranges for generation_data in func2]
    # --- FIM DA ETAPA DE NORMALIZAÇÃO ---

    def get_avg_objectives(normalized_func, source_name):
        """Calcula as médias dos objetivos e adiciona o nome da fonte."""
        avg_objectives = []
        for i, generation_data in enumerate(normalized_func):
            if generation_data.size > 0:
                means = np.mean(generation_data, axis=0)
                # Verifica se há o número esperado de objetivos (ex: 3)
                if len(means) >= 3:
                     avg_objectives.append({
                        'Geracao': i + 1,
                        'Fonte': source_name,
                        'F1': means[0],
                        'F2': means[1],
                        'F3': means[2]
                        # Adicione mais F's se necessário
                    })
        return avg_objectives

    # Calcula as médias para cada fonte
    avg_objectives1 = get_avg_objectives(normalized_func1, 'Fonte 1')
    avg_objectives2 = get_avg_objectives(normalized_func2, 'Fonte 2')

    # Combina os resultados em um único DataFrame
    all_avg_objectives = avg_objectives1 + avg_objectives2

    if not all_avg_objectives:
        print("Não foi possível calcular as médias dos objetivos.")
        return

    df = pd.DataFrame(all_avg_objectives)

    # Filtra o DataFrame para a geração desejada
    df_filtered = df[df['Geracao'] == generation_to_plot]

    if df_filtered.empty:
        print(f"Nenhum dado encontrado para a Geração {generation_to_plot}.")
        return


    # Prepara os dados para o formato que o plotly espera ("long format")
    df_melted = df_filtered.melt(id_vars=['Geracao', 'Fonte'],
                                 value_vars=['F1', 'F2', 'F3'],
                                 var_name='Objetivo',
                                 value_name='Valor Médio Normalizado')

    # Cria o gráfico de radar (spider chart)
    fig = px.line_polar(df_melted,
                        r='Valor Médio Normalizado',
                        theta='Objetivo',
                        color='Fonte',  # Usa a coluna 'Fonte' para definir as cores
                        line_close=True,
                        title=f'Comparação das Médias Normalizadas dos Objetivos (Geração {generation_to_plot})',
                        markers=True,
                        labels={"Fonte": "Fonte de Dados"},
                        range_r=[0, 1])  # Define o eixo do raio de 0 a 1

    # Salva o gráfico em um arquivo HTML interativo
    output_filename = f"radar_chart_comparativo_geracao_{generation_to_plot}.html"
    fig.write_html(output_filename)
    print(f"Gráfico de radar comparativo salvo como '{output_filename}'")


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Uso: python seu_script.py <arquivo1> <arquivo2> <geracao_para_plotar>")
        sys.exit(1)

    filename1 = sys.argv[1]
    filename2 = sys.argv[2]
    try:
        generation_to_plot = int(sys.argv[3])
    except ValueError:
        print("Erro: O número da geração deve ser um inteiro.")
        sys.exit(1)


    plot_radar_chart(filename1, filename2, generation_to_plot)