import os
import subprocess

# Parâmetros fixos
alpha = 0.5
population_size = 80
number_of_iterations = 500
tournament_size = 3

# Caminhos das pastas e arquivos
instances_dir = "./instances/40"
outputs_dir = "./outputs"
solutions_file = "./solutions/repeatedSolutions.txt"
recurrence_script = "./include/scripts/getRecurrenceTable.py"

# Cria a pasta de saída, se não existir
os.makedirs(outputs_dir, exist_ok=True)

# Lista todos os arquivos na pasta de instâncias
instance_files = [f for f in os.listdir(instances_dir) if os.path.isfile(os.path.join(instances_dir, f))]

# Executa o comando para cada instância
for instance_file in instance_files:
    instance_path = os.path.join(instances_dir, instance_file)
    output_file = os.path.join(outputs_dir, f"{instance_file}.txt")
    recurrence_file = os.path.join(outputs_dir, f"{instance_file}_recurrence.txt")
    
    # Comando para executar o algoritmo principal
    command = [
        "./MOVRPDD",
        instance_path,
        str(alpha),
        str(population_size),
        str(number_of_iterations),
        str(tournament_size)
    ]
    
    print(f"Executing: {' '.join(command)}")
    
    # Executa o comando principal e salva a saída
    with open(output_file, "w") as output:
        subprocess.run(command, stdout=output, stderr=output)
    
    # Comando para gerar a tabela de recorrência
    recurrence_command = [
        "python",
        recurrence_script,
        solutions_file,
        instance_file
    ]
    
    print(f"Generating recurrence table for: {instance_file}")
    
    # Executa o script de geração de tabela de recorrência e salva a saída
    with open(recurrence_file, "w") as recurrence_output:
        subprocess.run(recurrence_command, stdout=recurrence_output, stderr=recurrence_output)

print("Execuções e tabelas de recorrência concluídas. Verifique os resultados na pasta ./outputs.")
