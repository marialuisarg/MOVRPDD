import sys
import pandas as pd
import re
from collections import defaultdict

def processar_arquivo(arquivo):
    # Dicionário para armazenar as soluções e suas informações
    tabela_recurrencia = defaultdict(lambda: {"count": 0, "geracoes": []})
    
    with open(arquivo, "r") as f:
        for linha in f:
            # Expressão regular para capturar "Gen {num} - f1 | f2 | f3"
            match = re.match(r"Gen (\d+) - ([\d\.\-e]+) \| ([\d\.\-e]+) \| ([\d\.\-e]+)", linha)
            if match:
                geracao = int(match.group(1))
                solucao = (float(match.group(2)), float(match.group(3)), float(match.group(4)))
                
                # Atualiza o dicionário
                tabela_recurrencia[solucao]["count"] += 1
                tabela_recurrencia[solucao]["geracoes"].append(geracao)
    
    # Convertendo o dicionário para uma tabela pandas
    tabela = []
    for solucao, dados in tabela_recurrencia.items():
        tabela.append({
            "Solução (f1 | f2 | f3)": f"{solucao[0]} | {solucao[1]} | {solucao[2]}",
            "Número de ocorrências": dados["count"],
            "Gerações": ", ".join(map(str, sorted(dados["geracoes"])))
        })
    
    return pd.DataFrame(tabela).sort_values(by="Número de ocorrências", ascending=False)

if __name__ == '__main__':
    filename = sys.argv[1]
    outfile = "./recurrenceTables/" + sys.argv[2] + ".txt"
    
    table = processar_arquivo(filename)
    with open(outfile, "w") as f:
        f.write(table.to_string(index=False))
    
