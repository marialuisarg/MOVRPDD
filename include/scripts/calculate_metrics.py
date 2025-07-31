import numpy as np
from pymoo.indicators.hv import HV
from pymoo.indicators.igd import IGD
from pymoo.util.nds.non_dominated_sorting import NonDominatedSorting

def carregar_dados(caminho_arquivo):
    try:
        return np.loadtxt(caminho_arquivo)
    except Exception as e:
        print(f"Erro ao carregar o arquivo {caminho_arquivo}: {e}")
        return np.array([[]])

def calcular_c_metrica(A, B):
    if A.shape[1] == 0 or B.shape[1] == 0:
        return 0.0

    solucoes_em_B_dominadas = 0
    for sol_b in B:
        dominada = False
        for sol_a in A:
            if np.all(sol_a <= sol_b) and np.any(sol_a < sol_b):
                dominada = True
                break
        if dominada:
            solucoes_em_B_dominadas += 1
            
    return solucoes_em_B_dominadas / len(B)

caminho_a1 = './solutions/firstFront/frontGlobal/40_40_0.7_adpt.txt'
caminho_a2 = './solutions/firstFront/frontGlobal/40_40_0.7_adpt_bl.txt'
caminho_a3 = './solutions/firstFront/frontGlobal/40_40_0.7_lit_bl.txt'

A1_raw = carregar_dados(caminho_a1)
A2_raw = carregar_dados(caminho_a2)
A3_raw = carregar_dados(caminho_a3)

if A1_raw.size == 0 or A2_raw.size == 0 or A3_raw.size == 0:
    print("Um ou mais arquivos de dados estão vazios ou não foram encontrados. Encerrando.")
else:

    print("--- Iniciando Normalização Min-Max ---")

    todas_as_solucoes_raw = np.vstack([A1_raw, A2_raw, A3_raw])

    min_vals = todas_as_solucoes_raw.min(axis=0)
    max_vals = todas_as_solucoes_raw.max(axis=0)
    
    print(f"Valores Mínimos por Objetivo (antes da norma): {min_vals}")
    print(f"Valores Máximos por Objetivo (antes da norma): {max_vals}")

    range_vals = max_vals - min_vals
    range_vals[range_vals == 0] = 1 # Se a faixa for 0, evita DivByZero. A normalização será 0.

    A1_norm = (A1_raw - min_vals) / range_vals
    A2_norm = (A2_raw - min_vals) / range_vals
    A3_norm = (A3_raw - min_vals) / range_vals
    
    print("Normalização concluída. Todas as métricas serão calculadas com dados no intervalo [0, 1].\n")

    todas_as_solucoes_norm = np.vstack([A1_norm, A2_norm, A3_norm])
    nds = NonDominatedSorting()
    indices_nao_dominados = nds.do(todas_as_solucoes_norm, only_non_dominated_front=True)
    fronteira_referencia = todas_as_solucoes_norm[indices_nao_dominados]
    
    print(f"Total de soluções combinadas: {len(todas_as_solucoes_norm)}")
    print(f"Tamanho da Fronteira de Referência (não-dominadas globais): {len(fronteira_referencia)}\n")

    # --- Cálculo das Métricas (usando dados normalizados) ---

    # HIPERVOLUME (HV)
    ponto_referencia = np.array([1.1, 1.1, 1.1])
    
    print(f"Ponto de Referência para o HV (em escala normalizada): {ponto_referencia}")
    
    indicador_hv = HV(ref_point=ponto_referencia)
    hv_a1 = indicador_hv.do(A1_norm)
    hv_a2 = indicador_hv.do(A2_norm)
    hv_a3 = indicador_hv.do(A3_norm)

    # DISTÂNCIA GERACIONAL INVERTIDA (IGD)
    indicador_igd = IGD(pf=fronteira_referencia)
    igd_a1 = indicador_igd.do(A1_norm)
    igd_a2 = indicador_igd.do(A2_norm)
    igd_a3 = indicador_igd.do(A3_norm)

    # C-MÉTRICA (COBERTURA)
    c_a1_a2 = calcular_c_metrica(A1_norm, A2_norm)
    c_a2_a1 = calcular_c_metrica(A2_norm, A1_norm)
    c_a1_a3 = calcular_c_metrica(A1_norm, A3_norm)
    c_a3_a1 = calcular_c_metrica(A3_norm, A1_norm)
    c_a2_a3 = calcular_c_metrica(A2_norm, A3_norm)
    c_a3_a2 = calcular_c_metrica(A3_norm, A2_norm)


    # --- Exibição dos Resultados ---
    print("\n--- RESULTADOS DAS MÉTRICAS DE QUALIDADE (COM DADOS NORMALIZADOS) ---")
    print("Lembrete: HV (maior é melhor), IGD (menor é melhor)\n")

    print(f"ADPT:")
    print(f"  - Hipervolume (HV): {hv_a1:.4f}")
    print(f"  - IGD:              {igd_a1:.4f}\n")

    print(f"ADPT+BL:")
    print(f"  - Hipervolume (HV): {hv_a2:.4f}")
    print(f"  - IGD:              {igd_a2:.4f}\n")

    print(f"LIT+BL:")
    print(f"  - Hipervolume (HV): {hv_a3:.4f}")
    print(f"  - IGD:              {igd_a3:.4f}\n")

    print("--- Resultados da C-Métrica (Cobertura) ---")
    print("Lembrete: C(A, B) = % de B que é dominado por A\n")
    print(f"C(ADPT, ADPT+BL): {c_a1_a2:.2%} das soluções de ADPT+BL são dominadas pela ADPT.")
    print(f"C(ADPT+BL, ADPT): {c_a2_a1:.2%} das soluções de ADPT são dominadas pela ADPT+BL.\n")
    
    print(f"C(LIT+BL, ADPT+BL): {c_a3_a2:.2%} das soluções de ADPT+BL são dominadas pela LIT+BL.")
    print(f"C(ADPT+BL, LIT+BL): {c_a2_a3:.2%} das soluções de LIT+BL são dominadas pela ADPT+BL.\n")

    print(f"C(ADPT, LIT+BL): {c_a1_a3:.2%} das soluções de LIT+BL são dominadas pela Abordagem 1.")
    print(f"C(LIT+BL, ADPT): {c_a3_a1:.2%} das soluções de ADPT são dominadas pela LIT+BL.\n")
