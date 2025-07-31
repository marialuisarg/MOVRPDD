import os
from collections import defaultdict
import re

PASTA_ENTRADA = 'solutions/firstFront'
PASTA_SAIDA = 'solutions/firstFront/frontGlobal'
NUM_BLOCOS_ESPERADO = 500

def processar_arquivos():
    """
    Percorre uma pasta de entrada, extrai o último bloco de dados de cada arquivo
    e os agrupa em novos arquivos na pasta de saída com base no prefixo.
    """

    if not os.path.exists(PASTA_SAIDA):
        print(f"Criando pasta de saída em: '{PASTA_SAIDA}'")
        os.makedirs(PASTA_SAIDA)

    dados_agrupados = defaultdict(list)

    print(f"Lendo arquivos da pasta '{PASTA_ENTRADA}'...")

    for nome_arquivo in os.listdir(PASTA_ENTRADA):
        caminho_completo = os.path.join(PASTA_ENTRADA, nome_arquivo)

        if os.path.isfile(caminho_completo) and re.match(r'^.+_\d+\.txt$', nome_arquivo):
            
            print(f"  - Processando '{nome_arquivo}'...")
            
            prefixo = nome_arquivo.rsplit('_', 1)[0]

            try:
                with open(caminho_completo, 'r', encoding='utf-8') as f:
                    conteudo = f.read()

                blocos = conteudo.strip().split('\n\n')

                if len(blocos) == NUM_BLOCOS_ESPERADO:
                    ultimo_bloco = blocos[-1].strip()
                    dados_agrupados[prefixo].append(ultimo_bloco)
                else:
                    print(f"    AVISO: O arquivo '{nome_arquivo}' não contém os {NUM_BLOCOS_ESPERADO} blocos esperados (encontrados: {len(blocos)}). Pulando.")

            except Exception as e:
                print(f"    ERRO: Não foi possível ler o arquivo '{nome_arquivo}': {e}")

    print(f"\nSalvando arquivos de resultado na pasta '{PASTA_SAIDA}'...")
    if not dados_agrupados:
        print("Nenhum arquivo correspondente ao padrão foi encontrado para processar.")
        return

    for prefixo, lista_de_blocos in dados_agrupados.items():
        nome_saida = f"{prefixo}.txt"
        caminho_saida = os.path.join(PASTA_SAIDA, nome_saida)

        try:
            with open(caminho_saida, 'w', encoding='utf-8') as f_out:
                # Junta todos os blocos coletados, separados por uma quebra de linha
                conteudo_final = '\n'.join(lista_de_blocos)
                f_out.write(conteudo_final)
            print(f"  - Arquivo '{nome_saida}' salvo com sucesso.")
        except Exception as e:
            print(f"    ERRO: Não foi possível escrever o arquivo '{nome_saida}': {e}")
    
    print("\nProcessamento concluído!")

# --- EXECUÇÃO DO SCRIPT ---
if __name__ == '__main__':
    if not os.path.exists(PASTA_ENTRADA):
        print(f"ERRO: A pasta de entrada '{PASTA_ENTRADA}' não foi encontrada.")
    else:
        processar_arquivos()