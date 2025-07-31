import sys
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import numpy as np
import pandas as pd
import dash
from sklearn.preprocessing import MinMaxScaler
from dash import dcc, html
from dash.dependencies import Input, Output, State

# ==============================================================================
# FUNÇÕES DE PROCESSAMENTO DE DADOS
# ==============================================================================

def read_objectives(filename):
    """Lê os dados de objetivos de um arquivo, tratando erros."""
    try:
        with open(filename, 'r') as f:
            all_data = f.read().strip()
    except FileNotFoundError:
        raise FileNotFoundError(f"Erro: Arquivo '{filename}' não encontrado.")
    
    generations = all_data.split('\n\n')
    func = []
    for i, geracao in enumerate(generations):
        if geracao:
            try:
                functions = np.array([list(map(float, linha.split())) for linha in geracao.splitlines()])
                func.append(functions)
            except ValueError:
                print(f"Aviso: Erro de conversão na geração {i+1} do arquivo '{filename}'. Pulando.")
                continue
    return func

# --- Funções para gerar cada tipo de gráfico ---

def generate_evolution_figure(filepath, multiple):
    """Gera o gráfico de evolução por múltiplos."""
    # (Lógica da resposta anterior, sem alterações)
    func = read_objectives(filepath)
    if not func: return {}
    all_solutions = np.vstack([gen for gen in func if gen.size > 0])
    min_vals, max_vals = np.min(all_solutions, axis=0), np.max(all_solutions, axis=0)
    ranges = max_vals - min_vals
    ranges[ranges == 0] = 1
    normalized_func = [(gen - min_vals) / ranges for gen in func]
    avg_objectives = []
    for i, gen_data in enumerate(normalized_func):
        if gen_data.size > 0 and len(np.mean(gen_data, axis=0)) >= 3:
            means = np.mean(gen_data, axis=0)
            avg_objectives.append({'Geracao': i + 1, 'F1': means[0], 'F2': means[1], 'F3': means[2]})
    df = pd.DataFrame(avg_objectives)
    df_filtered = df[(df['Geracao'] == 1) | (df['Geracao'] % multiple == 0)]
    df_melted = df_filtered.melt(id_vars=['Geracao'], value_vars=['F1', 'F2', 'F3'],
                                 var_name='Objetivo', value_name='Valor Médio Normalizado')
    return px.line_polar(df_melted, r='Valor Médio Normalizado', theta='Objetivo', color='Geracao',
                         line_close=True, title=f'Evolução das Médias (Múltiplos de {multiple})',
                         markers=True, labels={"Geracao": "Geração"}, range_r=[0, 1])

def generate_comparison_figure(filepath1, filepath2, generation):
    """Gera o gráfico comparativo, normalizando os valores médios da geração específica."""
    # (Lógica da resposta anterior, sem alterações)
    func1, func2 = read_objectives(filepath1), read_objectives(filepath2)
    if len(func1) < generation or len(func2) < generation: raise ValueError(f"Geração {generation} não encontrada.")
    front1, front2 = func1[generation - 1], func2[generation - 1]
    if front1.size == 0 or front2.size == 0: raise ValueError(f"Geração {generation} sem dados válidos.")
    avg1, avg2 = np.mean(front1, axis=0), np.mean(front2, axis=0)
    avg_data_to_normalize = np.vstack([avg1, avg2])
    min_vals, max_vals = np.min(avg_data_to_normalize, axis=0), np.max(avg_data_to_normalize, axis=0)
    ranges = max_vals - min_vals
    ranges[ranges == 0] = 1
    norm_avg1, norm_avg2 = (avg1 - min_vals) / ranges, (avg2 - min_vals) / ranges
    df_to_plot = pd.DataFrame([
        {'Fonte': 'Arquivo 1', 'F1': norm_avg1[0], 'F2': norm_avg1[1], 'F3': norm_avg1[2]},
        {'Fonte': 'Arquivo 2', 'F1': norm_avg2[0], 'F2': norm_avg2[1], 'F3': norm_avg2[2]},
    ])
    df_melted = df_to_plot.melt(id_vars=['Fonte'], value_vars=['F1', 'F2', 'F3'],
                                var_name='Objetivo', value_name='Valor Médio Normalizado')
    return px.line_polar(df_melted, r='Valor Médio Normalizado', theta='Objetivo', color='Fonte',
                         line_close=True, title=f'Comparação de Médias Normalizadas (Geração {generation})',
                         markers=True, labels={"Fonte": "Fonte"}, range_r=[0, 1])

def generate_3d_scatter_figure(filepath, generation):
    """Gera o gráfico 3D de uma fronteira para uma geração específica."""
    # (Lógica da resposta anterior, sem alterações)
    func = read_objectives(filepath)
    if not func or len(func) < generation: raise ValueError(f"Geração {generation} não encontrada.")
    front = func[generation - 1]
    if front.size == 0: raise ValueError(f"Geração {generation} não contém dados válidos.")
    min_vals, max_vals = np.min(front, axis=0), np.max(front, axis=0)
    ranges = max_vals - min_vals
    ranges[ranges == 0] = 1
    norm_front = (front - min_vals) / ranges
    df_3d = pd.DataFrame(norm_front, columns=['F1', 'F2', 'F3'])
    fig = px.scatter_3d(df_3d, x='F1', y='F2', z='F3',
                        title=f'Fronteira de Pareto em 3D (Geração {generation})',
                        labels={'F1': 'Objetivo 1', 'F2': 'Objetivo 2', 'F3': 'Objetivo 3'})
    fig.update_traces(marker=dict(size=4)); fig.update_layout(margin=dict(l=0, r=0, b=0, t=40))
    return fig

def generate_detailed_radar_figure(filepath, generation):
    """Gera um radar chart com cada solução individual de uma geração."""
    # (Lógica da resposta anterior, sem alterações)
    func = read_objectives(filepath)
    if not func or len(func) < generation: raise ValueError(f"Geração {generation} não encontrada.")
    front_data = func[generation - 1]
    if front_data.size == 0: raise ValueError(f"Geração {generation} não contém dados válidos.")
    min_vals, max_vals = np.min(front_data, axis=0), np.max(front_data, axis=0)
    ranges = max_vals - min_vals
    ranges[ranges == 0] = 1
    norm_front = (front_data - min_vals) / ranges
    df_detailed = pd.DataFrame(norm_front, columns=['F1', 'F2', 'F3'])
    df_detailed['Solucao'] = [f'Sol. {i+1}' for i in range(len(df_detailed))]
    df_melted = df_detailed.melt(id_vars=['Solucao'], value_vars=['F1', 'F2', 'F3'],
                                 var_name='Objetivo', value_name='Valor Normalizado')
    return px.line_polar(df_melted, r='Valor Normalizado', theta='Objetivo', color='Solucao',
                         line_close=True, title=f'Soluções Individuais da Fronteira (Geração {generation})',
                         markers=True, labels={"Solucao": "Solução"})


def generate_2d_scatter_figure(filepath):
    """
    Lê um arquivo com todas as gerações, pega a última, normaliza seus dados,
    e plota dois gráficos de dispersão 2D (F1vsF2, F1vsF3).
    """
    # 1. Usa a função original para ler todas as gerações do arquivo
    func = read_objectives(filepath)
    if not func:
        raise ValueError("Arquivo não contém gerações válidas.")

    # 2. Pega apenas os dados da última geração
    last_front = func[-1]
    if last_front.size == 0:
        raise ValueError("A última geração no arquivo não contém dados válidos.")

    # 3. Normaliza os dados da última geração usando MinMaxScaler
    normalized_data = MinMaxScaler().fit_transform(last_front)
    
    df_2d = pd.DataFrame(normalized_data, columns=['F1', 'F2', 'F3'])
    
    # 4. Cria a figura com 2 subplots
    fig = make_subplots(rows=1, cols=2, subplot_titles=("F1 vs F2", "F1 vs F3"))

    fig.add_trace(go.Scatter(x=df_2d['F1'], y=df_2d['F2'], mode='markers'), row=1, col=1)
    fig.add_trace(go.Scatter(x=df_2d['F1'], y=df_2d['F3'], mode='markers'), row=1, col=2)

    # 5. Configura os eixos para irem de 0 a 1 e serem quadrados
    fig.update_xaxes(title_text="f1", range=[0, 1], row=1, col=1)
    fig.update_yaxes(title_text="f2", range=[0, 1], row=1, col=1, scaleanchor="x", scaleratio=1)
    fig.update_xaxes(title_text="f1", range=[0, 1], row=1, col=2)
    fig.update_yaxes(title_text="f3", range=[0, 1], row=1, col=2, scaleanchor="x2", scaleratio=1)
    
    fig.update_layout(
        title_text="Fronteira de Pareto 2D da Última Geração",
        showlegend=False
    )
    return fig

# ==============================================================================
# APLICAÇÃO DASH
# ==============================================================================
app = dash.Dash(__name__)

app.layout = html.Div([
    html.H1("Dashboard de Análise de Fronteiras de Pareto"),
    
    dcc.RadioItems(
        id='mode-selector',
        options=[
            {'label': 'Evolução por Múltiplos', 'value': 'evolucao'},
            {'label': 'Dominância entre Soluções', 'value': 'detailed_radar'},
            {'label': 'Comparativo da Evolução entre Abordagens', 'value': 'comparativo'},
            {'label': 'Visualização 3D da Fronteira', 'value': '3d_scatter'},
            {'label': 'Visualização 2D da Fronteira', 'value': '2d_scatter'},
        ],
        value='evolucao',
        labelStyle={'display': 'inline-block', 'margin-right': '20px'}
    ),
    
    # Divs de inputs (incluindo o novo)
    html.Div(id='evolucao-inputs', children=[
        html.Hr(), html.H3("Modo Evolução"),
        html.Label("Caminho do Arquivo:"), dcc.Input(id='evol-filepath-input', type='text', style={'width': '50%'}),
        html.Br(), html.Br(), html.Label("Exibir gerações múltiplas de:"),
        dcc.Input(id='evol-multiple-input', type='number', value=10, min=1, step=1),
    ]),
    html.Div(id='comparativo-inputs', children=[
        html.Hr(), html.H3("Modo Comparativo"),
        html.Label("Caminho do Arquivo 1:"), dcc.Input(id='comp-filepath1-input', type='text', style={'width': '50%'}),
        html.Br(), html.Br(), html.Label("Caminho do Arquivo 2:"),
        dcc.Input(id='comp-filepath2-input', type='text', style={'width': '50%'}),
        html.Br(), html.Br(), html.Label("Plotar Geração Específica:"),
        dcc.Input(id='comp-generation-input', type='number', value=100, min=1, step=1),
    ]),
    html.Div(id='scatter-3d-inputs', children=[
        html.Hr(), html.H3("Modo Visualização 3D"),
        html.Label("Caminho do Arquivo:"), dcc.Input(id='scatter-filepath-input', type='text', style={'width': '50%'}),
        html.Br(), html.Br(), html.Label("Plotar Geração Específica:"),
        dcc.Input(id='scatter-generation-input', type='number', value=40, min=1, step=1),
    ]),
    html.Div(id='detailed-radar-inputs', children=[
        html.Hr(), html.H3("Modo Fronteira 2D Detalhada"),
        html.Label("Caminho do Arquivo:"), dcc.Input(id='detailed-filepath-input', type='text', style={'width': '50%'}),
        html.Br(), html.Br(), html.Label("Plotar Geração Específica:"),
        dcc.Input(id='detailed-generation-input', type='number', value=1, min=1, step=1),
    ]),
    html.Div(id='scatter-2d-inputs', children=[ # <-- NOVO DIV
        html.Hr(), html.H3("Modo Fronteira 2D (Última Geração)"),
        html.Label("Caminho do Arquivo:"),
        dcc.Input(id='scatter-2d-filepath-input', type='text', placeholder='ex: ./resultados.txt', style={'width': '50%'}),
    ]),
    
    html.Br(),
    html.Button('Gerar Gráfico', id='submit-button', n_clicks=0),
    dcc.Loading(id="loading-spinner", type="circle", children=dcc.Graph(id='main-graph'))
])

# --- CALLBACKS ---

# Callback para mostrar/esconder os inputs
@app.callback(
    Output('evolucao-inputs', 'style'),
    Output('comparativo-inputs', 'style'),
    Output('scatter-3d-inputs', 'style'),
    Output('detailed-radar-inputs', 'style'),
    Output('scatter-2d-inputs', 'style'), # <-- NOVO OUTPUT
    Input('mode-selector', 'value')
)
def toggle_input_visibility(selected_mode):
    return (
        {'display': 'block' if selected_mode == 'evolucao' else 'none'},
        {'display': 'block' if selected_mode == 'comparativo' else 'none'},
        {'display': 'block' if selected_mode == '3d_scatter' else 'none'},
        {'display': 'block' if selected_mode == 'detailed_radar' else 'none'},
        {'display': 'block' if selected_mode == '2d_scatter' else 'none'}
    )

# Callback principal para gerar o gráfico
@app.callback(
    Output('main-graph', 'figure'),
    Input('submit-button', 'n_clicks'),
    [State('mode-selector', 'value'),
     State('evol-filepath-input', 'value'), State('evol-multiple-input', 'value'),
     State('comp-filepath1-input', 'value'), State('comp-filepath2-input', 'value'), State('comp-generation-input', 'value'),
     State('scatter-filepath-input', 'value'), State('scatter-generation-input', 'value'),
     State('detailed-filepath-input', 'value'), State('detailed-generation-input', 'value'),
     State('scatter-2d-filepath-input', 'value')]
)
def update_graph(n_clicks, mode, evol_file, evol_mult, comp_file1, comp_file2, comp_gen, scatter_3d_file, scatter_3d_gen, detailed_file, detailed_gen, scatter_2d_file):
    if n_clicks == 0: return dash.no_update
    
    # Cria um dicionário para guardar a figura final ou uma figura de erro
    fig = go.Figure()

    try:
        if mode == 'evolucao':
            if not evol_file or not evol_mult: raise ValueError("Preencha todos os campos para o modo Evolução.")
            fig = generate_evolution_figure(evol_file, evol_mult)
        elif mode == 'comparativo':
            if not comp_file1 or not comp_file2 or not comp_gen: raise ValueError("Preencha todos os campos para o modo Comparativo.")
            fig = generate_comparison_figure(comp_file1, comp_file2, comp_gen)
        elif mode == '3d_scatter':
            if not scatter_3d_file or not scatter_3d_gen: raise ValueError("Preencha todos os campos para o modo Visualização 3D.")
            fig = generate_3d_scatter_figure(scatter_3d_file, scatter_3d_gen)
        elif mode == 'detailed_radar':
            if not detailed_file or not detailed_gen: raise ValueError("Preencha todos os campos para o modo Fronteira 2D Detalhada.")
            fig = generate_detailed_radar_figure(detailed_file, detailed_gen)
        elif mode == '2d_scatter': # <-- NOVA LÓGICA
            if not scatter_2d_file: raise ValueError("Preencha o caminho do arquivo para o modo Fronteira 2D.")
            fig = generate_2d_scatter_figure(scatter_2d_file)
            
    except Exception as e:
        # Para erros, mostra um layout vazio com o texto do erro no centro
        fig.update_layout(
            title_text="Erro ao Gerar Gráfico",
            xaxis={'visible': False}, yaxis={'visible': False},
            annotations=[dict(text=str(e), showarrow=False, xref="paper", yref="paper", x=0.5, y=0.5)]
        )
    
    return fig

if __name__ == '__main__':
    app.run_server(debug=True)