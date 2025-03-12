#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/intercalacao2f.h"
#include "../include/leitura.h"

#define MAX_MEMORIA 20 // Tamanho máximo da memória disponível

// Estrutura para o heap de seleção por substituição
typedef struct {
    float nota;          // Nota do registro
    long posicao;        // Posição original no vetor de registros
    int ciclo;           // Número da corrida (ciclo) a que o elemento pertence
} HeapNode;

// Flag de ordem
#define ORDEM_ASCENDENTE 0
#define ORDEM_DESCENDENTE 1

// Função auxiliar para comparar duas estruturas NotaPosicao
int comparar_nota_posicao(const void *a, const void *b, int ordem) {
    float nota_a = ((NotaPosicao *)a)->nota;
    float nota_b = ((NotaPosicao *)b)->nota;
    
    if (ordem == ORDEM_ASCENDENTE) {
        if (nota_a < nota_b) return -1;
        if (nota_a > nota_b) return 1;
    } else { // ORDEM_DESCENDENTE
        if (nota_a > nota_b) return -1;
        if (nota_a < nota_b) return 1;
    }
    return 0;
}

// Funções para manipulação do heap
void trocar_nos(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void descer_no_heap(HeapNode *heap, int i, int n, int ordem) {
    int escolhido = i;
    int esquerda = 2 * i + 1;
    int direita = 2 * i + 2;

    // Comparações baseadas na ordem solicitada
    if (ordem == ORDEM_ASCENDENTE) {
        // Compara com o filho da esquerda se existir (procurando o menor)
        if (esquerda < n && (heap[esquerda].ciclo < heap[escolhido].ciclo || 
                          (heap[esquerda].ciclo == heap[escolhido].ciclo && 
                           heap[esquerda].nota < heap[escolhido].nota))) {
            escolhido = esquerda;
        }

        // Compara com o filho da direita se existir
        if (direita < n && (heap[direita].ciclo < heap[escolhido].ciclo || 
                         (heap[direita].ciclo == heap[escolhido].ciclo && 
                          heap[direita].nota < heap[escolhido].nota))) {
            escolhido = direita;
        }
    } else { // ORDEM_DESCENDENTE
        // Compara com o filho da esquerda se existir (procurando o maior)
        if (esquerda < n && (heap[esquerda].ciclo < heap[escolhido].ciclo || 
                          (heap[esquerda].ciclo == heap[escolhido].ciclo && 
                           heap[esquerda].nota > heap[escolhido].nota))) {
            escolhido = esquerda;
        }

        // Compara com o filho da direita se existir
        if (direita < n && (heap[direita].ciclo < heap[escolhido].ciclo || 
                         (heap[direita].ciclo == heap[escolhido].ciclo && 
                          heap[direita].nota > heap[escolhido].nota))) {
            escolhido = direita;
        }
    }

    // Se o escolhido não for o próprio nó, troca e continua descendo
    if (escolhido != i) {
        trocar_nos(&heap[i], &heap[escolhido]);
        descer_no_heap(heap, escolhido, n, ordem);
    }
}

void construir_heap(HeapNode *heap, int n, int ordem) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        descer_no_heap(heap, i, n, ordem);
    }
}

// Função que implementa a seleção por substituição para criar corridas iniciais
int selecao_por_substituicao(Registro *registros, int quantidade, NotaPosicao *fita1, 
                             NotaPosicao *fita2, int *tam_fita1, int *tam_fita2, 
                             Metricas *stats, int ordem) {
    HeapNode *heap = (HeapNode *)malloc(MAX_MEMORIA * sizeof(HeapNode));
    if (!heap) return 0;

    int num_ciclos = 0;       // Contador de corridas geradas
    int heap_size = 0;        // Tamanho atual do heap
    int prox_registro = 0;    // Próximo registro a ser lido
    int ciclo_atual = 0;      // Corrida atual
    *tam_fita1 = 0;
    *tam_fita2 = 0;
    
    clock_t inicio, fim;
    iniciar_tempo(&inicio);

    // Inicializar o heap com os primeiros MAX_MEMORIA registros
    for (int i = 0; i < MAX_MEMORIA && prox_registro < quantidade; i++) {
        heap[i].nota = registros[prox_registro].nota;
        heap[i].posicao = prox_registro;
        heap[i].ciclo = 0;     // Todos começam na corrida 0
        prox_registro++;
        heap_size++;
        stats->leituras_pre++;
    }

    // Construir o heap inicial
    construir_heap(heap, heap_size, ordem);

    // Enquanto houver elementos no heap
    while (heap_size > 0) {
        // O elemento mais adequado está na raiz do heap
        NotaPosicao saida;
        saida.nota = heap[0].nota;
        saida.posicao = heap[0].posicao;
        
        // Determinar em qual fita colocar a saída com base na corrida atual
        if (ciclo_atual % 2 == 0) {
            fita1[(*tam_fita1)++] = saida;
        } else {
            fita2[(*tam_fita2)++] = saida;
        }
        stats->escritas_pre++;
        
        // Se ainda houver registros a serem processados
        if (prox_registro < quantidade) {
            // Substitui o elemento removido pelo próximo do input
            float prox_nota = registros[prox_registro].nota;
            
            // Verifica se o próximo registro pode continuar na mesma corrida
            // Baseado na ordem (ascendente ou descendente)
            int pode_continuar = 0;
            if (ordem == ORDEM_ASCENDENTE) {
                pode_continuar = (prox_nota >= saida.nota);
            } else { // ORDEM_DESCENDENTE
                pode_continuar = (prox_nota <= saida.nota);
            }
            
            if (pode_continuar) {
                heap[0].nota = prox_nota;
                heap[0].posicao = prox_registro;
                heap[0].ciclo = ciclo_atual;
            } else {
                // Caso contrário, vai para a próxima corrida
                heap[0].nota = prox_nota;
                heap[0].posicao = prox_registro;
                heap[0].ciclo = ciclo_atual + 1;
                
                // Se todos os elementos estão na próxima corrida, avançamos a corrida atual
                int todos_nova_corrida = 1;
                for (int i = 1; i < heap_size; i++) {
                    if (heap[i].ciclo == ciclo_atual) {
                        todos_nova_corrida = 0;
                        break;
                    }
                }
                
                if (todos_nova_corrida) {
                    ciclo_atual++;
                    num_ciclos++;
                }
            }
            
            prox_registro++;
            stats->leituras_pre++;
            
            // Restaurar a propriedade do heap
            descer_no_heap(heap, 0, heap_size, ordem);
            stats->comparacoes_pre += heap_size * 2; // Aproximação grosseira
        } else {
            // Não há mais registros de entrada, remove-se o elemento do heap
            heap[0] = heap[heap_size - 1];
            heap_size--;
            
            // Se o heap não estiver vazio, restaura a propriedade do heap
            if (heap_size > 0) {
                descer_no_heap(heap, 0, heap_size, ordem);
                stats->comparacoes_pre += heap_size * 2;
            }
            
            // Verifica se todos os elementos restantes são de uma nova corrida
            int todos_nova_corrida = 1;
            for (int i = 0; i < heap_size; i++) {
                if (heap[i].ciclo == ciclo_atual) {
                    todos_nova_corrida = 0;
                    break;
                }
            }
            
            if (todos_nova_corrida && heap_size > 0) {
                ciclo_atual++;
                num_ciclos++;
            }
        }
    }
    
    // Incrementa num_ciclos para contar a última corrida
    num_ciclos++;
    
    free(heap);
    finalizar_tempo(&inicio, &fim, &stats->tempo_execucao_pre);

    
    return num_ciclos;
}

// Função para intercalar duas corridas
void intercalar_corridas(NotaPosicao *resultado, NotaPosicao *fita1, int *idx1, int fim1, 
                        NotaPosicao *fita2, int *idx2, int fim2, int *pos_resultado, 
                        Metricas *stats, int ordem) {
    while (*idx1 <= fim1 && *idx2 <= fim2) {
        stats->comparacoes_pos++;
        
        int selecionar_fita1 = 0;
        if (ordem == ORDEM_ASCENDENTE) {
            selecionar_fita1 = (fita1[*idx1].nota <= fita2[*idx2].nota);
        } else { // ORDEM_DESCENDENTE
            selecionar_fita1 = (fita1[*idx1].nota >= fita2[*idx2].nota);
        }
        
        if (selecionar_fita1) {
            resultado[(*pos_resultado)++] = fita1[(*idx1)++];
        } else {
            resultado[(*pos_resultado)++] = fita2[(*idx2)++];
        }
        stats->leituras_pos++;
    }
    
    // Copia os elementos restantes da primeira fita, se houver
    while (*idx1 <= fim1) {
        resultado[(*pos_resultado)++] = fita1[(*idx1)++];
        stats->leituras_pos++;
    }
    
    // Copia os elementos restantes da segunda fita, se houver
    while (*idx2 <= fim2) {
        resultado[(*pos_resultado)++] = fita2[(*idx2)++];
        stats->leituras_pos++;
    }
}

// Função principal de intercalação balanceada com seleção por substituição
void intercalacao_balanceada_2f(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats, int ordem) {
    Registro *registros = NULL;
    clock_t inicio, fim;
    iniciar_tempo(&inicio);

    // Read records
    ler_binario("./data/registros.bin", &registros, quantidade);
    if (!registros) {
        printf("Erro ao ler registros.\n");
        return;
    }

    // Allocate "tapes"
    NotaPosicao *fita1 = malloc(quantidade * sizeof(NotaPosicao));
    NotaPosicao *fita2 = malloc(quantidade * sizeof(NotaPosicao));
    NotaPosicao *resultado = malloc(quantidade * sizeof(NotaPosicao));
    int tam_fita1, tam_fita2;

    if (!fita1 || !fita2 || !resultado) {
        printf("Erro ao alocar memória para fitas.\n");
        free(registros);
        if (fita1) free(fita1);
        if (fita2) free(fita2);
        if (resultado) free(resultado);
        return;
    }

    // Generate initial ciclos
    int num_ciclos = selecao_por_substituicao(registros, quantidade, fita1, fita2, 
                                             &tam_fita1, &tam_fita2, stats, ordem);

    // Vector for ordered positions
    long *posicoes_ordenadas = malloc(quantidade * sizeof(long));
    if (!posicoes_ordenadas) {
        printf("Erro ao alocar memória para posições ordenadas.\n");
        free(registros);
        free(fita1);
        free(fita2);
        free(resultado);
        return;
    }

    // Start intercalation
    iniciar_tempo(&inicio);
    
    // If only one ciclo was generated, it's already sorted
    if (num_ciclos <= 1) {
        if (tam_fita1 > 0) {
            for (int i = 0; i < tam_fita1; i++) {
                posicoes_ordenadas[i] = fita1[i].posicao;
                stats->leituras_pos++;
            }
        } else if (tam_fita2 > 0) {
            for (int i = 0; i < tam_fita2; i++) {
                posicoes_ordenadas[i] = fita2[i].posicao;
                stats->leituras_pos++;
            }
        }
    } else {
        // Multiple ciclos - need to merge them
        // Create arrays to track ciclo information explicitly
        int *ciclos_fita1 = malloc((num_ciclos + 1) * sizeof(int));
        int *ciclos_fita2 = malloc((num_ciclos + 1) * sizeof(int));
        int num_ciclos_fita1 = 0;
        int num_ciclos_fita2 = 0;
        
        // Initialize ciclo boundaries - distribute ciclos evenly
        for (int i = 0; i < num_ciclos; i++) {
            if (i % 2 == 0 && tam_fita1 > 0) {
                // Even ciclos go to tape 1
                if (num_ciclos_fita1 == 0) {
                    ciclos_fita1[num_ciclos_fita1++] = 0;
                }
                
                // Find the fim of this ciclo by checking for order breaks
                int ciclo_fim = tam_fita1 - 1;
                for (int j = ciclos_fita1[num_ciclos_fita1-1]; j < tam_fita1 - 1; j++) {
                    if ((ordem == ORDEM_ASCENDENTE && fita1[j].nota > fita1[j+1].nota) ||
                        (ordem == ORDEM_DESCENDENTE && fita1[j].nota < fita1[j+1].nota)) {
                        ciclo_fim = j;
                        break;
                    }
                }
                ciclos_fita1[num_ciclos_fita1++] = ciclo_fim + 1;
            } else if (tam_fita2 > 0) {
                // Odd ciclos go to tape 2
                if (num_ciclos_fita2 == 0) {
                    ciclos_fita2[num_ciclos_fita2++] = 0;
                }
                
                // Find the fim of this ciclo
                int ciclo_fim = tam_fita2 - 1;
                for (int j = ciclos_fita2[num_ciclos_fita2-1]; j < tam_fita2 - 1; j++) {
                    if ((ordem == ORDEM_ASCENDENTE && fita2[j].nota > fita2[j+1].nota) ||
                        (ordem == ORDEM_DESCENDENTE && fita2[j].nota < fita2[j+1].nota)) {
                        ciclo_fim = j;
                        break;
                    }
                }
                ciclos_fita2[num_ciclos_fita2++] = ciclo_fim + 1;
            }
        }
        
        // Fix last ciclo boundaries
        if (num_ciclos_fita1 > 0) {
            ciclos_fita1[num_ciclos_fita1] = tam_fita1;
        }
        if (num_ciclos_fita2 > 0) {
            ciclos_fita2[num_ciclos_fita2] = tam_fita2;
        }
        
        // Merge until only one ciclo remains
        while (num_ciclos_fita1 + num_ciclos_fita2 > 1) {
            int pos_resultado = 0;
            int next_fita1 = 0;
            int next_fita2 = 0;
            
            // Merge pairs of ciclos
            while (next_fita1 < num_ciclos_fita1 && next_fita2 < num_ciclos_fita2) {
                // Get ciclo boundaries
                int inicio_fita1 = ciclos_fita1[next_fita1];
                int fim_fita1 = ciclos_fita1[next_fita1 + 1] - 1;
                int inicio_fita2 = ciclos_fita2[next_fita2];
                int fim_fita2 = ciclos_fita2[next_fita2 + 1] - 1;
                
                // Merge these two ciclos
                int idx1 = inicio_fita1;
                int idx2 = inicio_fita2;
                intercalar_corridas(resultado, fita1, &idx1, fim_fita1, 
                                   fita2, &idx2, fim_fita2, 
                                   &pos_resultado, stats, ordem);
                
                next_fita1++;
                next_fita2++;
            }
            
            // Handle leftover ciclos from fita1
            while (next_fita1 < num_ciclos_fita1) {
                int inicio = ciclos_fita1[next_fita1];
                int fim = ciclos_fita1[next_fita1 + 1] - 1;
                
                for (int i = inicio; i <= fim; i++) {
                    resultado[pos_resultado++] = fita1[i];
                    stats->leituras_pos++;
                    stats->escritas_pos++;
                }
                
                next_fita1++;
            }
            
            // Handle leftover ciclos from fita2
            while (next_fita2 < num_ciclos_fita2) {
                int inicio = ciclos_fita2[next_fita2];
                int fim = ciclos_fita2[next_fita2 + 1] - 1;
                
                for (int i = inicio; i <= fim; i++) {
                    resultado[pos_resultado++] = fita2[i];
                    stats->leituras_pos++;
                    stats->escritas_pos++;
                }
                
                next_fita2++;
            }
            
            // Prepare for next iteration - distribute ciclos back to tapes
            tam_fita1 = 0;
            tam_fita2 = 0;
            num_ciclos_fita1 = 0;
            num_ciclos_fita2 = 0;
            
            // Reset ciclo trackers
            ciclos_fita1[0] = 0;
            ciclos_fita2[0] = 0;
            
            // Scan resultado to find ciclos and distribute them
            int ciclo_start = 0;
            for (int i = 1; i < pos_resultado; i++) {
                // Detect when a break in order happens
                if ((ordem == ORDEM_ASCENDENTE && resultado[i-1].nota > resultado[i].nota) ||
                    (ordem == ORDEM_DESCENDENTE && resultado[i-1].nota < resultado[i].nota)) {
                    // fim of a ciclo detected
                    if (num_ciclos_fita1 <= num_ciclos_fita2) {
                        // Copy to fita1
                        for (int j = ciclo_start; j < i; j++) {
                            fita1[tam_fita1++] = resultado[j];
                        }
                        ciclos_fita1[num_ciclos_fita1 + 1] = tam_fita1;
                        num_ciclos_fita1++;
                    } else {
                        // Copy to fita2
                        for (int j = ciclo_start; j < i; j++) {
                            fita2[tam_fita2++] = resultado[j];
                        }
                        ciclos_fita2[num_ciclos_fita2 + 1] = tam_fita2;
                        num_ciclos_fita2++;
                    }
                    ciclo_start = i;
                }
            }
            
            // Handle the last ciclo
            if (ciclo_start < pos_resultado) {
                if (num_ciclos_fita1 <= num_ciclos_fita2) {
                    // Copy to fita1
                    for (int j = ciclo_start; j < pos_resultado; j++) {
                        fita1[tam_fita1++] = resultado[j];
                    }
                    ciclos_fita1[num_ciclos_fita1 + 1] = tam_fita1;
                    num_ciclos_fita1++;
                } else {
                    // Copy to fita2
                    for (int j = ciclo_start; j < pos_resultado; j++) {
                        fita2[tam_fita2++] = resultado[j];
                    }
                    ciclos_fita2[num_ciclos_fita2 + 1] = tam_fita2;
                    num_ciclos_fita2++;
                }
            }
        }
        
        // Final result is in one of the tapes
        if (num_ciclos_fita1 == 1) {
            for (int i = 0; i < tam_fita1; i++) {
                posicoes_ordenadas[i] = fita1[i].posicao;
            }
        } else if (num_ciclos_fita2 == 1) {
            for (int i = 0; i < tam_fita2; i++) {
                posicoes_ordenadas[i] = fita2[i].posicao;
            }
        }
        
        free(ciclos_fita1);
        free(ciclos_fita2);
    }
    
    finalizar_tempo(&inicio, &fim, &stats->tempo_execucao_pos);

    // Log metrics
    const char* ordem_str = (ordem == ORDEM_ASCENDENTE) ? "Ascendente" : "Descendente";
    char nome_algoritmo[100];
    sprintf(nome_algoritmo, "Intercalacao 2f - Selecao Substituicao (%s)", ordem_str);
    log_metricas(nome_algoritmo, quantidade, situacao == 1 ? "1" : situacao == 2 ? "2" : "3", *stats);

    // Print sorted records
    printf("\nRegistros ordenados por nota (ordem %s):\n", ordem_str);
    for (int i = 0; i < quantidade; i++) {
        print_registro(&registros[posicoes_ordenadas[i]]);
    } 

    // Cleanup
    free(posicoes_ordenadas);
    free(fita1);
    free(fita2);
    free(resultado);
    free(registros);
}

// Wrapper para a versão ascendente (compatibilidade com código existente)
void intercalacao_balanceada_2f_ascendente(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats) {
    intercalacao_balanceada_2f(nome_arquivo, quantidade, situacao, stats, ORDEM_ASCENDENTE);
}

// Wrapper para a versão descendente
void intercalacao_balanceada_2f_descendente(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats) {
    intercalacao_balanceada_2f(nome_arquivo, quantidade, situacao, stats, ORDEM_DESCENDENTE);
}