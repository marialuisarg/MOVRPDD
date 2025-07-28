import subprocess
import pathlib
import argparse
import sys
import re

def main():

    parser = argparse.ArgumentParser(
        description="Roda um executável C++ para múltiplas instâncias, registra o tempo de CPU e renomeia arquivos de saída.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    
    parser.add_argument("executable", help="Caminho para o executável C++.")
    parser.add_argument("instance_dir", help="Diretório contendo os arquivos das instâncias.")
    parser.add_argument(
        "execution_type",
        help="O tipo de execução a ser registrada.",
        # Restringe as opções para garantir consistência
        choices=["0", "1", "2", "3"]
    )
    parser.add_argument("population_size", type=int, help="Tamanho da população para o algoritmo genético.")
    parser.add_argument("num_iterations", type=int, help="Número de gerações para o algoritmo genético.")
    parser.add_argument("tournament_size", type=int, help="Tamanho do torneio para seleção no algoritmo genético.")
    parser.add_argument("seed", type=int, help="Semente para o gerador de números aleatórios.")
    parser.add_argument("-n", "--repetitions", type=int, default=20, help="Número de execuções por instância (padrão: 20).")
    parser.add_argument("-o", "--output_log", default="resultados_cpu.csv", help="Arquivo de log CSV (padrão: resultados_cpu.csv).")
    args = parser.parse_args()

    executable_path = pathlib.Path(args.executable)
    instance_path = pathlib.Path(args.instance_dir)

    if not executable_path.is_file():
        print(f"Erro: O executável não foi encontrado em '{executable_path}'")
        sys.exit(1)
    
    if not instance_path.is_dir():
        print(f"Erro: O diretório de instâncias não foi encontrado em '{instance_path}'")
        sys.exit(1)


    file_type_mapping = {
        "LIT": "lit",
        "LIT + BL": "lit_bl",
        "ADPT": "adpt",
        "ADPT + BL": "adpt_bl"
    }
    file_safe_type = file_type_mapping[args.execution_type]

    log_path = pathlib.Path(args.output_log)

    if not log_path.exists():
        with open(log_path, "w") as log_file:
            log_file.write("Instancia,Tipo_Execucao,Num_Execucao,Tempo_CPU_Segundos\n")

    print(f"Encontradas {len(list(instance_path.glob('40_*')) + list(instance_path.glob('80_*')))} instâncias.")
    print(f"Tipo de Execução: {args.execution_type}")
    print(f"Resultados serão anexados em '{log_path}'")

    all_files = sorted(instance_path.glob('40_*')) + sorted(instance_path.glob('80_*'))

    for instance_file in all_files:
        instance_base_name = instance_file.stem
        print(f"\n{'='*20} Processando Instância: {instance_base_name} {'='*20}")
        
        for i in range(1, args.repetitions + 1):
            command = [str(executable_path), str(instance_file)]
            print(f"  -> Execução {i}/{args.repetitions}...", end="", flush=True)

            try:
                result = subprocess.run(command, capture_output=True, text=True, check=False)

                if result.returncode == 0:
                    print(" Sucesso!")
                    
                    cpu_time = "NA"
                    match = re.search(r"CPU computing time: (\d+\.?\d*)", result.stdout)
                    if match:
                        cpu_time = match.group(1)
                    else:
                        print("      Aviso: Não foi possível encontrar o tempo de CPU na saída do programa.")

                    with open(log_path, "a") as log_file:
                        log_file.write(f"{instance_base_name},{args.execution_type},{i},{cpu_time}\n")

                    original_ff_path = pathlib.Path(f"firstFront_{instance_base_name}.txt")

                    new_ff_path = pathlib.Path(f"firstFront_{instance_base_name}_{file_safe_type}_{i}.txt")
                    
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