import sys
import plotly.express as px
import numpy as np
import pandas as pd
import dash
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
    """Gera o gráfico comparativo para uma geração específica."""

    func1, func2 = read_objectives(filepath1), read_objectives(filepath2)
    if not func1 or not func2: return {}
    all_solutions = np.vstack([gen for gen in func1 if gen.size > 0] + [gen for gen in func2 if gen.size > 0])
    if all_solutions.size == 0: return {}
    min_vals, max_vals = np.min(all_solutions, axis=0), np.max(all_solutions, axis=0)
    ranges = max_vals - min_vals
    ranges[ranges == 0] = 1
    norm_func1, norm_func2 = [(gen - min_vals) / ranges for gen in func1], [(gen - min_vals) / ranges for gen in func2]
    def get_avg(nf, src):
        avgs = []
        for i, gd in enumerate(nf):
            if gd.size > 0 and len(np.mean(gd, axis=0)) >= 3:
                m = np.mean(gd, axis=0)
                avgs.append({'Geracao': i + 1, 'Fonte': src, 'F1': m[0], 'F2': m[1], 'F3': m[2]})
        return avgs
    df = pd.DataFrame(get_avg(norm_func1, 'Arquivo 1') + get_avg(norm_func2, 'Arquivo 2'))
    df_filtered = df[df['Geracao'] == generation]
    if df_filtered.empty: raise ValueError(f"Nenhum dado para a Geração {generation}.")
    df_melted = df_filtered.melt(id_vars=['Geracao', 'Fonte'], value_vars=['F1', 'F2', 'F3'],
                                 var_name='Objetivo', value_name='Valor Médio Normalizado')
    return px.line_polar(df_melted, r='Valor Médio Normalizado', theta='Objetivo', color='Fonte',
                         line_close=True, title=f'Comparação de Médias (Geração {generation})',
                         markers=True, labels={"Fonte": "Fonte"}, range_r=[0, 1])

def generate_3d_scatter_figure(filepath, generation):
    """Gera o gráfico 3D de uma fronteira para uma geração específica."""
    func = read_objectives(filepath)
    if not func or len(func) < generation:
        raise ValueError(f"Geração {generation} não encontrada ou arquivo inválido.")
    
    # Pega os dados da geração específica
    front = func[generation - 1]
    if front.size == 0:
        raise ValueError(f"Geração {generation} não contém dados válidos.")

    # Normalização local (apenas dentro da geração selecionada)
    min_vals, max_vals = np.min(front, axis=0), np.max(front, axis=0)
    ranges = max_vals - min_vals
    ranges[ranges == 0] = 1
    norm_front = (front - min_vals) / ranges
    
    # Cria o DataFrame para o Plotly
    df_3d = pd.DataFrame(norm_front, columns=['F1', 'F2', 'F3'])
    
    # Cria o gráfico 3D com Plotly Express
    fig = px.scatter_3d(
        df_3d, x='F1', y='F2', z='F3',
        title=f'Fronteira de Pareto em 3D (Geração {generation})',
        labels={'F1': 'Objetivo 1', 'F2': 'Objetivo 2', 'F3': 'Objetivo 3'}
    )
    fig.update_traces(marker=dict(size=4))
    fig.update_layout(margin=dict(l=0, r=0, b=0, t=40))
    return fig

# ==============================================================================
# APLICAÇÃO DASH
# ==============================================================================
app = dash.Dash(__name__)

app.layout = html.Div([
    html.H1("Dashboard de Análise de Fronteiras de Pareto"),
    
    html.Label("Selecione o modo de visualização:"),
    dcc.RadioItems(
        id='mode-selector',
        options=[
            {'label': 'Evolução por Múltiplos', 'value': 'evolucao'},
            {'label': 'Comparativo de Geração', 'value': 'comparativo'},
            {'label': 'Visualização 3D da Fronteira', 'value': '3d_scatter'}, # <-- NOVA OPÇÃO
        ],
        value='evolucao',
        labelStyle={'display': 'inline-block', 'margin-right': '20px'}
    ),
    
    # --- Div para os inputs do modo "Evolução" ---
    html.Div(id='evolucao-inputs', children=[
        html.Hr(), html.H3("Modo Evolução"),
        html.Label("Caminho do Arquivo:"),
        dcc.Input(id='evol-filepath-input', type='text', placeholder='ex: ./resultados.txt', style={'width': '50%'}),
        html.Br(), html.Br(),
        html.Label("Exibir gerações múltiplas de:"),
        dcc.Input(id='evol-multiple-input', type='number', value=10, min=1, step=1),
    ]),
    
    # --- Div para os inputs do modo "Comparativo" ---
    html.Div(id='comparativo-inputs', children=[
        html.Hr(), html.H3("Modo Comparativo"),
        html.Label("Caminho do Arquivo 1:"),
        dcc.Input(id='comp-filepath1-input', type='text', placeholder='ex: ./fonte_a.txt', style={'width': '50%'}),
        html.Br(), html.Br(),
        html.Label("Caminho do Arquivo 2:"),
        dcc.Input(id='comp-filepath2-input', type='text', placeholder='ex: ./fonte_b.txt', style={'width': '50%'}),
        html.Br(), html.Br(),
        html.Label("Plotar Geração Específica:"),
        dcc.Input(id='comp-generation-input', type='number', value=100, min=1, step=1),
    ]),
    
    # --- Div para os inputs do modo "Visualização 3D" ---
    html.Div(id='scatter-3d-inputs', children=[ # <-- NOVO DIV
        html.Hr(), html.H3("Modo Visualização 3D"),
        html.Label("Caminho do Arquivo:"),
        dcc.Input(id='scatter-filepath-input', type='text', placeholder='ex: ./resultados.txt', style={'width': '50%'}),
        html.Br(), html.Br(),
        html.Label("Plotar Geração Específica:"),
        dcc.Input(id='scatter-generation-input', type='number', value=40, min=1, step=1),
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
    Output('scatter-3d-inputs', 'style'), # <-- NOVO OUTPUT
    Input('mode-selector', 'value')
)
def toggle_input_visibility(selected_mode):
    show_evol = {'display': 'block'} if selected_mode == 'evolucao' else {'display': 'none'}
    show_comp = {'display': 'block'} if selected_mode == 'comparativo' else {'display': 'none'}
    show_3d = {'display': 'block'} if selected_mode == '3d_scatter' else {'display': 'none'}
    return show_evol, show_comp, show_3d

# Callback principal para gerar o gráfico
@app.callback(
    Output('main-graph', 'figure'),
    Input('submit-button', 'n_clicks'),
    # Adiciona os inputs do novo modo como State
    [State('mode-selector', 'value'),
     State('evol-filepath-input', 'value'), State('evol-multiple-input', 'value'),
     State('comp-filepath1-input', 'value'), State('comp-filepath2-input', 'value'), State('comp-generation-input', 'value'),
     State('scatter-filepath-input', 'value'), State('scatter-generation-input', 'value')]
)
def update_graph(n_clicks, mode, evol_file, evol_mult, comp_file1, comp_file2, comp_gen, scatter_file, scatter_gen):
    if n_clicks == 0:
        return dash.no_update

    try:
        if mode == 'evolucao':
            if not evol_file or not evol_mult: raise ValueError("Preencha todos os campos para o modo Evolução.")
            return generate_evolution_figure(evol_file, evol_mult)
        
        elif mode == 'comparativo':
            if not comp_file1 or not comp_file2 or not comp_gen: raise ValueError("Preencha todos os campos para o modo Comparativo.")
            return generate_comparison_figure(comp_file1, comp_file2, comp_gen)
            
        elif mode == '3d_scatter': # <-- NOVA LÓGICA
            if not scatter_file or not scatter_gen: raise ValueError("Preencha todos os campos para o modo Visualização 3D.")
            return generate_3d_scatter_figure(scatter_file, scatter_gen)
            
    except Exception as e:
        fig_error = px.line_polar()
        fig_error.update_layout(
            title_text="Erro ao Gerar Gráfico",
            annotations=[dict(text=str(e), showarrow=False, xref="paper", yref="paper", x=0.5, y=0.5)]
        )
        return fig_error

if __name__ == '__main__':
    app.run_server(debug=True)