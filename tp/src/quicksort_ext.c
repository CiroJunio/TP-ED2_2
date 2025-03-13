#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/quicksort_ext.h"

#define MEMORIA_INTERNA 15  // Quantidade máxima de registros em memória interna

// Função que divide o arquivo em dois arquivos menores baseado em um pivô
void particionar_arquivo(char *arquivo_entrada, char *arquivo_menores, char *arquivo_maiores, float pivo, Metricas* stats) {
    FILE *entrada = fopen(arquivo_entrada, "rb");
    FILE *menores = fopen(arquivo_menores, "wb");
    FILE *maiores = fopen(arquivo_maiores, "wb");
    
    if (!entrada || !menores || !maiores) {
        printf("Erro ao abrir arquivos para particionamento.\n");
        return;
    }
    
    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, entrada) == 1) {
        stats->leituras_pos++;
        stats->comparacoes_pos++;
        
        if (reg.nota <= pivo) {
            fwrite(&reg, sizeof(Registro), 1, menores);
            stats->escritas_pos++;
        } else {
            fwrite(&reg, sizeof(Registro), 1, maiores);
            stats->escritas_pos++;
        }
    }
    
    fclose(entrada);
    fclose(menores);
    fclose(maiores);
}

// Conta o número de registros em um arquivo
int contar_registros(char *arquivo) {
    FILE *fp = fopen(arquivo, "rb");
    if (!fp) return 0;
    
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fclose(fp);
    
    return tamanho / sizeof(Registro);
}

// Função para selecionar um pivô baseado na amostragem de registros
float selecionar_pivo(char *arquivo, int situacao, Metricas* stats) {
    int num_registros = contar_registros(arquivo);
    if (num_registros <= 0) return 0.0;
    
    // Define o tamanho da amostra (até MEMORIA_INTERNA registros)
    int tamanho_amostra = (num_registros < MEMORIA_INTERNA) ? num_registros : MEMORIA_INTERNA;
    
    // Aloca memória para a amostra
    Registro *amostra = (Registro *) malloc(tamanho_amostra * sizeof(Registro));
    if (!amostra) return 0.0;
    
    // Lê uma amostra de registros do arquivo
    FILE *fp = fopen(arquivo, "rb");
    if (!fp) {
        free(amostra);
        return 0.0;
    }
    
    // Seleciona registros distribuídos pelo arquivo para obter uma boa amostra
    int intervalo = num_registros / tamanho_amostra;
    for (int i = 0; i < tamanho_amostra; i++) {
        fseek(fp, (i * intervalo) * sizeof(Registro), SEEK_SET);
        fread(&amostra[i], sizeof(Registro), 1, fp);
        stats->leituras_pos++;
    }
    
    fclose(fp);
    
    // Ordena a amostra usando um algoritmo simples
    for (int i = 0; i < tamanho_amostra - 1; i++) {
        for (int j = 0; j < tamanho_amostra - i - 1; j++) {
            stats->comparacoes_pos++;
            if ((situacao == 1 && amostra[j].nota > amostra[j+1].nota) ||
                (situacao == 2 && amostra[j].nota < amostra[j+1].nota)) {
                Registro temp = amostra[j];
                amostra[j] = amostra[j+1];
                amostra[j+1] = temp;
                stats->escritas_pos++;
            }
        }
    }
    
    // Seleciona o elemento do meio como pivô
    float pivo = amostra[tamanho_amostra / 2].nota;
    
    free(amostra);
    return pivo;
}

// Função que une dois arquivos ordenados em um único arquivo ordenado
void mesclar_arquivos(char *arquivo_saida, char *arquivo1, char *arquivo2, int situacao, Metricas* stats) {
    FILE *saida = fopen(arquivo_saida, "wb");
    FILE *f1 = fopen(arquivo1, "rb");
    FILE *f2 = fopen(arquivo2, "rb");
    
    if (!saida || (!f1 && !f2)) {
        printf("Erro ao abrir arquivos para mesclagem.\n");
        if (saida) fclose(saida);
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return;
    }
    
    // Se apenas um dos arquivos existe, copia-o para a saída
    if (!f1) {
        Registro reg;
        while (fread(&reg, sizeof(Registro), 1, f2) == 1) {
            stats->leituras_pos++;
            fwrite(&reg, sizeof(Registro), 1, saida);
            stats->escritas_pos++;
        }
        fclose(f2);
        fclose(saida);
        return;
    } else if (!f2) {
        Registro reg;
        while (fread(&reg, sizeof(Registro), 1, f1) == 1) {
            stats->leituras_pos++;
            fwrite(&reg, sizeof(Registro), 1, saida);
            stats->escritas_pos++;
        }
        fclose(f1);
        fclose(saida);
        return;
    }
    
    // Lê o primeiro registro de cada arquivo
    Registro reg1, reg2;
    int tem_reg1 = (fread(&reg1, sizeof(Registro), 1, f1) == 1);
    int tem_reg2 = (fread(&reg2, sizeof(Registro), 1, f2) == 1);
    stats->leituras_pos += (tem_reg1 + tem_reg2);
    
    // Mescla os arquivos
    while (tem_reg1 && tem_reg2) {
        stats->comparacoes_pos++;
        int escrever_reg1;
        
        if (situacao == 1) { // Ascendente
            escrever_reg1 = (reg1.nota <= reg2.nota);
        } else if (situacao == 2) { // Descendente
            escrever_reg1 = (reg1.nota >= reg2.nota);
        } else { // Situação aleatória (por simplicidade, mantém ascendente)
            escrever_reg1 = (reg1.nota <= reg2.nota);
        }
        
        if (escrever_reg1) {
            fwrite(&reg1, sizeof(Registro), 1, saida);
            stats->escritas_pos++;
            tem_reg1 = (fread(&reg1, sizeof(Registro), 1, f1) == 1);
            if (tem_reg1) stats->leituras_pos++;
        } else {
            fwrite(&reg2, sizeof(Registro), 1, saida);
            stats->escritas_pos++;
            tem_reg2 = (fread(&reg2, sizeof(Registro), 1, f2) == 1);
            if (tem_reg2) stats->leituras_pos++;
        }
    }
    
    // Escreve os registros restantes do primeiro arquivo
    while (tem_reg1) {
        fwrite(&reg1, sizeof(Registro), 1, saida);
        stats->escritas_pos++;
        tem_reg1 = (fread(&reg1, sizeof(Registro), 1, f1) == 1);
        if (tem_reg1) stats->leituras_pos++;
    }
    
    // Escreve os registros restantes do segundo arquivo
    while (tem_reg2) {
        fwrite(&reg2, sizeof(Registro), 1, saida);
        stats->escritas_pos++;
        tem_reg2 = (fread(&reg2, sizeof(Registro), 1, f2) == 1);
        if (tem_reg2) stats->leituras_pos++;
    }
    
    fclose(f1);
    fclose(f2);
    fclose(saida);
}

// Implementação recursiva do QuickSort Externo
void quicksort_externo_recursivo(char *arquivo, int situacao, Metricas* stats) {
    int num_registros = contar_registros(arquivo);
    
    // Caso base: arquivo com 0 ou 1 registro já está ordenado
    if (num_registros <= 1) {
        return;
    }
    
    // Caso o número de registros seja pequeno o suficiente para ordenação em memória
    if (num_registros <= MEMORIA_INTERNA) {
        // Aloca memória para os registros
        Registro *registros = (Registro *)malloc(num_registros * sizeof(Registro));
        if (!registros) {
            printf("Erro ao alocar memória para ordenação interna.\n");
            return;
        }
        
        // Lê todos os registros
        FILE *fp = fopen(arquivo, "rb");
        if (!fp) {
            free(registros);
            return;
        }
        
        fread(registros, sizeof(Registro), num_registros, fp);
        fclose(fp);
        stats->leituras_pos += num_registros;
        
        // Ordena os registros usando o algoritmo interno
        for (int i = 0; i < num_registros - 1; i++) {
            for (int j = 0; j < num_registros - i - 1; j++) {
                stats->comparacoes_pos++;
                int trocar;
                
                if (situacao == 1) { // Ascendente
                    trocar = (registros[j].nota > registros[j+1].nota);
                } else if (situacao == 2) { // Descendente
                    trocar = (registros[j].nota < registros[j+1].nota);
                } else { // Aleatório (simplificado como ascendente)
                    trocar = (registros[j].nota > registros[j+1].nota);
                }
                
                if (trocar) {
                    Registro temp = registros[j];
                    registros[j] = registros[j+1];
                    registros[j+1] = temp;
                    stats->escritas_pos++;
                }
            }
        }
        
        // Escreve os registros ordenados de volta para o arquivo
        fp = fopen(arquivo, "wb");
        if (!fp) {
            free(registros);
            return;
        }
        
        fwrite(registros, sizeof(Registro), num_registros, fp);
        fclose(fp);
        stats->escritas_pos += num_registros;
        
        free(registros);
        return;
    }
    
    // Seleciona um pivô
    float pivo = selecionar_pivo(arquivo, situacao, stats);
    
    // Cria nomes para arquivos temporários
    char arquivo_menores[100], arquivo_maiores[100];
    sprintf(arquivo_menores, "%s_menores", arquivo);
    sprintf(arquivo_maiores, "%s_maiores", arquivo);
    
    // Particiona o arquivo com base no pivô
    particionar_arquivo(arquivo, arquivo_menores, arquivo_maiores, pivo, stats);
    
    // Ordenação recursiva das partições
    quicksort_externo_recursivo(arquivo_menores, situacao, stats);
    quicksort_externo_recursivo(arquivo_maiores, situacao, stats);
    
    // Mescla as partições ordenadas
    mesclar_arquivos(arquivo, arquivo_menores, arquivo_maiores, situacao, stats);
    
    // Remove os arquivos temporários
    remove(arquivo_menores);
    remove(arquivo_maiores);
}

// Função principal para executar o QuickSort Externo
void quicksort_externo(char *arquivo, int quantidade, int situacao, int imprime) {
    Metricas stats = {0, 0, 0, 0.0, 0, 0, 0, 0.0};
    clock_t inicio, fim;
    
    char arquivo_temp[100];
    sprintf(arquivo_temp, "%s_temp", arquivo);
    
    // Cria uma cópia do arquivo para trabalhar
    FILE *entrada = fopen(arquivo, "rb");
    FILE *temp = fopen(arquivo_temp, "wb");
    if (!entrada || !temp) {
        printf("Erro ao abrir arquivos para cópia.\n");
        if (entrada) fclose(entrada);
        if (temp) fclose(temp);
        return;
    }
    
    iniciar_tempo(&inicio);
    
    // Copia os primeiros 'quantidade' registros
    Registro reg;
    int contador = 0;
    while (contador < quantidade && fread(&reg, sizeof(Registro), 1, entrada) == 1) {
        fwrite(&reg, sizeof(Registro), 1, temp);
        contador++;
        stats.leituras_pre++;
    }
    
    fclose(entrada);
    fclose(temp);
    
    finalizar_tempo(&inicio, &fim, &stats.tempo_execucao_pre);
    
    // Executa o quicksort externo
    iniciar_tempo(&inicio);
    quicksort_externo_recursivo(arquivo_temp, situacao, &stats);
    finalizar_tempo(&inicio, &fim, &stats.tempo_execucao_pos);
    
    // Exibe os registros ordenados

    if (imprime == 1) {
        FILE *resultado = fopen(arquivo_temp, "rb");
        if (resultado) {
            while (fread(&reg, sizeof(Registro), 1, resultado) == 1) {
                print_registro(&reg);
            }
            fclose(resultado);
        }
    }
    const char *situacao_txt = (situacao == 1) ? "Ascendente" : (situacao == 2) ? "Descendente" : "Aleatório";
    log_metricas("QuickSort Externo", quantidade, situacao_txt, stats);
    
    // Remove o arquivo temporário
    remove(arquivo_temp);
}