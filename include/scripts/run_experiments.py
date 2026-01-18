import subprocess
import pathlib
import argparse
import sys
import re

def main():

    parser = argparse.ArgumentParser(
        description="Roda um executável C++ para múltiplas instâncias, registra o tempo de CPU e a seed, e renomeia arquivos de saída.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    
    parser.add_argument("executable", help="Caminho para o executável C++.")
    parser.add_argument("instance_dir", help="Diretório contendo os arquivos das instâncias.")
    parser.add_argument("population_size", type=int, help="Tamanho da população para o algoritmo genético.")
    parser.add_argument("num_iterations", type=int, help="Número de gerações para o algoritmo genético.")
    parser.add_argument("tournament_size", type=int, help="Tamanho do torneio para seleção no algoritmo genético.")
    parser.add_argument("-n", "--repetitions", type=int, default=15, help="Número de execuções por instância (padrão: 15).")
    parser.add_argument("-o", "--output_log", default="resultados_cpu.csv", help="Arquivo de log CSV (padrão: resultados_cpu.csv).")
    args = parser.parse_args()

    executable_path = pathlib.Path(args.executable).resolve()
    instance_path = pathlib.Path(args.instance_dir)

    if not executable_path.is_file():
        print(f"Erro: O executável não foi encontrado em '{executable_path}'")
        sys.exit(1)
    
    if not instance_path.is_dir():
        print(f"Erro: O diretório de instâncias não foi encontrado em '{instance_path}'")
        sys.exit(1)

    # Tipos de execução que serão rodados
    execution_types_to_run = ["LIT+BL"]

    file_type_mapping = {
        "LIT+BL": "lit_bl"
    }
    
    log_path = pathlib.Path(args.output_log)

    # Cria o arquivo de log com o cabeçalho correto se ele não existir
    if not log_path.exists():
        with open(log_path, "w") as log_file:
            log_file.write("Instancia,Tipo_Execucao,Num_Execucao,Seed,Tempo_CPU_Segundos\n")

    print(f"Encontradas {len(list(instance_path.glob('80*')))} instâncias.")
    print(f"Resultados serão anexados em '{log_path}'")

    all_files = sorted(instance_path.glob('80*'))

    # Laço principal sobre os tipos de execução
    for exec_type in execution_types_to_run:
        print(f"\n{'#'*25} Iniciando Tipo de Execução: {exec_type} {'#'*25}")
        file_safe_type = file_type_mapping[exec_type]

        for instance_file in all_files:
            nome_completo = instance_file.name 
            instance_base_name = nome_completo.removesuffix('.txt')
            print(f"\n{'='*20} Processando Instância: {instance_base_name} {'='*20}")
            
            for i in range(1, args.repetitions + 1):
                # Monta o comando sem o argumento da seed
                command = [
                    str(executable_path), 
                    str(instance_file),
                    exec_type,
                    str(args.population_size),
                    str(args.num_iterations),
                    str(args.tournament_size)
                ]
                
                print(f"  -> Execução {i}/{args.repetitions}...", end="", flush=True)

                try:
                    result = subprocess.run(command, capture_output=True, text=True, check=False, encoding='utf-8')

                    if result.returncode == 0:
                        print(" Sucesso!")
                        
                        # Extrai o tempo de CPU da saída
                        cpu_time = "NA"
                        match_time = re.search(r"CPU computing time: (\d+\.?\d*)", result.stdout)
                        if match_time:
                            cpu_time = match_time.group(1)
                        else:
                            print("      Aviso: Não foi possível encontrar o tempo de CPU na saída do programa.")

                        # Extrai a seed usada da saída
                        seed_value = "NA"
                        match_seed = re.search(r"Using seed: (\d+)", result.stdout)
                        if match_seed:
                            seed_value = match_seed.group(1)
                        else:
                            print("      Aviso: Não foi possível encontrar a seed na saída do programa.")

                        # Escreve a linha completa no CSV
                        with open(log_path, "a") as log_file:
                            log_file.write(f"{instance_base_name}\t{exec_type}\t{i}\t{seed_value}\t{cpu_time}\n")

                        # Renomeia o arquivo de saída do front
                        original_ff_path = pathlib.Path(f"solutions/generations/firstFront_{instance_base_name}.txt")
                        new_ff_path = pathlib.Path(f"solutions/firstFront/{instance_base_name}_{file_safe_type}_{i}.txt")
                        
                        # Garante que o diretório de soluções exista
                        new_ff_path.parent.mkdir(parents=True, exist_ok=True)
                        
                        if original_ff_path.exists():
                            original_ff_path.rename(new_ff_path)
                        else:
                            print(f"      Aviso: O arquivo de saída '{original_ff_path}' não foi encontrado para ser renomeado.")

                    else:
                        print(f" Falha! (Código de erro: {result.returncode})")
                        print(f"      Erro reportado: {result.stderr.strip()}")

                except Exception as e:
                    print(f"Ocorreu um erro inesperado: {e}")

    print(f"\nTodos os experimentos foram concluídos. Verifique o arquivo '{log_path}'.")

if __name__ == "__main__":
    main()