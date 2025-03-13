import subprocess
import matplotlib.pyplot as plt

def run_pesquisa(metodo, quantidade, situacao):
    """
    Executa o comando de ordenação no terminal e retorna os dados de pós-processamento.
    """
    comando = f"./ordena {metodo} {quantidade} {situacao}".strip()
    arquivo_saida = f"saida_ordena_{quantidade}_registros.txt"
    
    try:
        # Executar o comando e redirecionar a saída para o arquivo
        with open(arquivo_saida, "w") as f:
            subprocess.run(comando, shell=True, stdout=f, stderr=f, check=True)
        
        # Ler o arquivo para extrair as informações de pós-processamento
        with open(arquivo_saida, "r") as f:
            output = f.read()
        
        # Depuração: Mostrar a saída bruta para verificar o conteúdo
        print(f"Saída bruta para {quantidade} registros:\n{output}\n{'-'*50}")
        
        leituras = None
        comparacoes = None
        dados = {}
        in_pos_processamento = False

        # Processar a saída linha por linha
        for line in output.splitlines():
            line = line.strip()  # Remove espaços em branco
            
            # Identificar o início da seção de pós-processamento
            if "Métricas de Pós-processamento:" in line:
                in_pos_processamento = True
                continue
            
            # Extrair apenas se estivermos na seção de pós-processamento
            if in_pos_processamento:
                if "Leituras:" in line:
                    try:
                        leituras = int(line.split("Leituras:")[1].strip())
                        dados["leituras"] = leituras
                    except (IndexError, ValueError) as e:
                        print(f"Erro ao extrair Leituras em '{line}': {e}")
                elif "Comparações:" in line:
                    try:
                        comparacoes = int(line.split("Comparações:")[1].strip())
                        dados["comparacoes"] = comparacoes
                    except (IndexError, ValueError) as e:
                        print(f"Erro ao extrair Comparações em '{line}': {e}")
        
        # Verificar se os dados foram extraídos corretamente
        if leituras is None or comparacoes is None:
            raise Exception("Falha ao extrair métricas de pós-processamento.")
        
        return dados
    
    except subprocess.CalledProcessError as e:
        print(f"Erro ao executar o comando '{comando}': {e}")
        return None
    except Exception as e:
        print(f"Erro ao processar {quantidade} registros: {e}")
        return None

def gerar_grafico(quantidades):
    """
    Gera um gráfico onde:
    - X é o número de leituras (pós-processamento)
    - Y é o número de comparações (pós-processamento)
    - Cada linha corresponde a uma quantidade de registros
    """
    plt.figure(figsize=(10, 6))
    has_data = False  # Flag para verificar se há dados a plotar

    for quantidade in quantidades:
        arquivo_saida = f"saida_ordena_{quantidade}_registros.txt"
        
        try:
            with open(arquivo_saida, "r") as f:
                output = f.read()
            
            leituras = None
            comparacoes = None
            in_pos_processamento = False
            
            # Processar a saída linha por linha
            for line in output.splitlines():
                line = line.strip()
                
                if "Métricas de Pós-processamento:" in line:
                    in_pos_processamento = True
                    continue
                
                if in_pos_processamento:
                    if "Leituras:" in line:
                        try:
                            leituras = int(line.split("Leituras:")[1].strip())
                        except (IndexError, ValueError) as e:
                            print(f"Erro ao extrair Leituras em '{line}' (arquivo {arquivo_saida}): {e}")
                    elif "Comparações:" in line:
                        try:
                            comparacoes = int(line.split("Comparações:")[1].strip())
                        except (IndexError, ValueError) as e:
                            print(f"Erro ao extrair Comparações em '{line}' (arquivo {arquivo_saida}): {e}")
            
            if leituras is not None and comparacoes is not None:
                # Plotar a linha para essa quantidade de registros
                plt.plot([leituras], [comparacoes], marker='o', label=f"{quantidade} Registros")
                plt.plot([0, leituras], [0, comparacoes], linestyle='--', color='gray', alpha=0.5)
                has_data = True
            else:
                print(f"Falha ao extrair métricas de {arquivo_saida}")
        
        except FileNotFoundError:
            print(f"Arquivo {arquivo_saida} não encontrado.")
        except Exception as e:
            print(f"Erro ao ler arquivo {arquivo_saida}: {e}")
    
    if has_data:
        # Personalizar o título e os rótulos dos eixos
        plt.title("Comparações vs Leituras (Pós-processamento)")
        plt.xlabel("Leituras")
        plt.ylabel("Comparações")
        plt.grid(True)
        plt.legend()

        # Melhorar a exibição dos valores no eixo X e Y
        ax = plt.gca()
        ax.xaxis.set_major_formatter(plt.FuncFormatter(lambda x, _: f'{x:,.0f}'))
        ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda y, _: f'{y:,.0f}'))

        # Ajustar layout e salvar o gráfico
        plt.tight_layout()
        plt.savefig("grafico_comparacoes_leituras_pos.png")
        print("Gráfico salvo como grafico_comparacoes_leituras_pos.png")
    else:
        print("Nenhum dado válido para gerar o gráfico.")

def main():
    # Rodar o Makefile para compilar o programa
    print("Compilando o programa...")
    try:
        subprocess.run("make", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Erro ao compilar o programa: {e}")
        return
    
    # Solicitar ao usuário a escolha do método
    try:
        metodo = int(input("Escolha o método (1- 2F Fitas, 2- F + 1 Fitas, 3- Quicksort Externo): "))
    except ValueError:
        print("Entrada inválida. Por favor, insira um número entre 1 e 3.")
        return
    
    # Mapeamento dos nomes dos métodos
    metodos_nomes = {
        1: "2F Fitas",
        2: "F + 1 Fitas",
        3: "Quicksort Externo",
    }
    
    metodo_nome = metodos_nomes.get(metodo, "Método Desconhecido")
    
    # Configurações de teste
    quantidades = [100, 200, 2000]  # Ajuste conforme necessário
    situacao = 1  # Situação fixa conforme exemplo
    
    resultados = []
    
    for quantidade in quantidades:
        print(f"Executando método {metodo_nome} com {quantidade} registros...")
        dados = run_pesquisa(metodo, quantidade, situacao)
        if dados is not None:
            resultados.append(dados)
    
    # Gerar gráfico comparativo
    gerar_grafico(quantidades)

if __name__ == "__main__":
    main()