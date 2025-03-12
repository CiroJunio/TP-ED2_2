import subprocess
import time
import matplotlib.pyplot as plt

def run_pesquisa(metodo, quantidade, situacao, chave, mostrar_chaves=False):
    """
    Executa o comando de pesquisa no terminal, escreve em um arquivo e retorna os dados de saída.
    """
    opcional = "-P" if mostrar_chaves else ""
    comando = f"./pesquisa {metodo} {quantidade} {situacao} {chave} {opcional}".strip()
    
    # Nome do arquivo onde será armazenada a saída (diferente para cada quantidade)
    arquivo_saida = f"saida_pesquisa_{quantidade}_registros.txt"
    
    try:
        # Executar o comando e redirecionar a saída para o arquivo
        with open(arquivo_saida, "w") as f:
            subprocess.run(comando, shell=True, stdout=f, stderr=f, check=True)
        
        # Ler o arquivo para extrair as informações de transferências e comparações
        with open(arquivo_saida, "r") as f:
            output = f.read()
        
        transferencias = None
        comparacoes = None
        dados = {}

        # Processar a saída para extrair apenas transferências e comparações
# Processar a saída para extrair transferências e comparações
        for line in output.splitlines():
            if "Transferências:" in line:  # Ajustado para o formato correto
                transferencias = int(line.split("Transferências:")[1].strip())
                dados["transferencias"] = transferencias
            elif "Comparações:" in line:  # Ajustado para o formato correto
                comparacoes = int(line.split("Comparações:")[1].strip())
                dados["comparacoes"] = comparacoes

        
        # Verificar se os dados foram extraídos corretamente
        if transferencias is None or comparacoes is None:
            raise Exception("Falha ao extrair dados corretamente.")
        
        # Reescrever o arquivo com os dados extraídos
        with open(arquivo_saida, "a") as f:
            f.write(f"\nResultados extraídos:\nTransferências: {transferencias}\n")
            f.write(f"Comparações: {comparacoes}\n")
        
        return dados
    
    except Exception as e:
        print(f"Erro: {e}")
        return None

def gerar_grafico(quantidades):
    """
    Gera um gráfico onde:
    - X é o número de transferências
    - Y é o número de comparações
    - Cada linha corresponde a uma quantidade de registros
    """
    plt.figure(figsize=(10, 6))

    # Para cada quantidade de registros, vamos extrair transferências e comparações
    for quantidade in quantidades:
        arquivo_saida = f"saida_pesquisa_{quantidade}_registros.txt"
        
        try:
            with open(arquivo_saida, "r") as f:
                output = f.read()
            
            transferencias = []
            comparacoes = []
            
            for line in output.splitlines():
                if "Transferências" in line:
                    transferencias.append(int(line.split("Transferências:")[1].strip()))
                elif "Comparações" in line:
                    comparacoes.append(int(line.split("Comparações:")[1].strip()))
            
            if transferencias and comparacoes:
                # Plotar a linha para essa quantidade de registros
                plt.plot(transferencias, comparacoes, marker='o', label=f"{quantidade} Registros")

                # Adicionar uma linha que vai de (0, 0) até o último ponto (sem rótulo)
                plt.plot(
                    [0, transferencias[-1]], 
                    [0, comparacoes[-1]], 
                    linestyle='--', color='gray', alpha=0.5
                )
        
        except Exception as e:
            print(f"Erro ao ler arquivo {arquivo_saida}: {e}")
    
    # Personalizar o título e os rótulos dos eixos
    plt.title("Comparações vs Transferências")
    plt.xlabel("Transferências")
    plt.ylabel("Comparações")
    plt.grid(True)
    plt.legend()

    # Melhorar a exibição dos valores no eixo X e Y
    ax = plt.gca()
    ax.xaxis.set_major_formatter(plt.FuncFormatter(lambda x, _: f'{x:,.0f}'))  # Exibir números grandes no eixo X com separador de milhar
    ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda y, _: f'{y:,.0f}'))  # Exibir números grandes no eixo Y com separador de milhar

    # Ajustar layout e salvar o gráfico
    plt.tight_layout()
    plt.savefig("grafico_comparacoes_transferencias.png")  # Salva o gráfico com o nome fixo
    print("Gráfico salvo como grafico_comparacoes_transferencias.png")

def main():
    # Rodar o Makefile para compilar o programa
    print("Compilando o programa...")
    subprocess.run("make", shell=True, check=True)
    
    # Solicitar ao usuário a escolha do método
    metodo = int(input("Escolha o método (1- Acesso Sequencial Indexado, 2- Árvore Binária, 3- Árvore B, 4- Árvore B*): "))
    
    # Mapeamento dos nomes dos métodos
    metodos_nomes = {
        1: "Acesso Sequencial Indexado",
        2: "Árvore Binária",
        3: "Árvore B",
        4: "Árvore B*"
    }
    
    metodo_nome = metodos_nomes.get(metodo, "Método Desconhecido")
    
    # Configurações de teste
    quantidades = [100, 200, 2000, 20000, 200000, 1000000]
    situacao = 3  # Arquivo ordenado ascendentemente, decrescentemente e aleatoriamente
    chave = 100
    
    resultados = []
    
    for quantidade in quantidades:
        print(f"Executando método {metodo_nome} com {quantidade} registros...")
        dados = run_pesquisa(metodo, quantidade, situacao, chave)
        if dados is not None:
            resultados.append(dados)
    
    # Gerar gráfico comparativo
    gerar_grafico(quantidades)

if __name__ == "__main__":
    main()
