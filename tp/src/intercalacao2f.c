#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/intercalacao2f.h"
#include "../include/leitura.h"
#define MAX_MEMORIA 20 // Tamanho máximo da memória disponível (quantidade máxima de registros na memória principal)

// Estrutura para o heap de seleção por substituição
// Cada nó do heap contém uma nota, a posição original do registro e o ciclo a que pertence
typedef struct {
    float nota;          // Nota do registro (valor usado para ordenação)
    long posicao;        // Posição original no vetor de registros (para recuperar o registro completo depois)
    int ciclo;           // Número da corrida (ciclo) a que o elemento pertence (usado para separar as corridas)
} HeapNode;

// Constantes que definem a ordem de ordenação
#define ORDEM_ASCENDENTE 0  // Para ordenar do menor para o maior
#define ORDEM_DESCENDENTE 1 // Para ordenar do maior para o menor

// Função auxiliar para comparar duas estruturas NotaPosicao
// Retorna -1 se a < b, 1 se a > b, e 0 se iguais, considerando a ordem especificada
int comparar_nota_posicao(const void *a, const void *b, int ordem) {
    float nota_a = ((NotaPosicao *)a)->nota;
    float nota_b = ((NotaPosicao *)b)->nota;
    
    if (ordem == ORDEM_ASCENDENTE) {
        // Na ordem ascendente, a < b retorna -1 (a vem antes)
        if (nota_a < nota_b) return -1;
        if (nota_a > nota_b) return 1;
    } else { // ORDEM_DESCENDENTE
        // Na ordem descendente, a > b retorna -1 (a vem antes)
        if (nota_a > nota_b) return -1;
        if (nota_a < nota_b) return 1;
    }
    return 0; // Notas iguais
}

// Funções para manipulação do heap
// Troca dois nós do heap
void trocar_nos(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

// Função para "descer" um nó no heap, mantendo a propriedade do heap
// i: índice do nó a ser ajustado, n: tamanho do heap, ordem: ascendente ou descendente
void descer_no_heap(HeapNode *heap, int i, int n, int ordem) {
    int escolhido = i;
    int esquerda = 2 * i + 1; // Índice do filho da esquerda
    int direita = 2 * i + 2;  // Índice do filho da direita
    
    // Comparações baseadas na ordem solicitada
    if (ordem == ORDEM_ASCENDENTE) {
        // Compara com o filho da esquerda se existir (procurando o menor)
        // Primeiro critério: ciclo menor tem prioridade
        // Segundo critério (se mesmo ciclo): menor nota tem prioridade
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
        // Primeiro critério: ciclo menor tem prioridade
        // Segundo critério (se mesmo ciclo): maior nota tem prioridade
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
        descer_no_heap(heap, escolhido, n, ordem); // Recursivamente continua descendo
    }
}

// Constrói um heap a partir de um array de nós
// Transforma o array em um heap válido (min-heap ou max-heap, dependendo da ordem)
void construir_heap(HeapNode *heap, int n, int ordem) {
    // Começa a partir do último nó não-folha e vai descendo cada nó
    for (int i = n / 2 - 1; i >= 0; i--) {
        descer_no_heap(heap, i, n, ordem);
    }
}

// Função que implementa a seleção por substituição para criar corridas iniciais
// Divide os registros em corridas ordenadas e distribui nas fitas
int selecao_por_substituicao(Registro *registros, int quantidade, NotaPosicao *fita1, 
                             NotaPosicao *fita2, int *tam_fita1, int *tam_fita2, 
                             Metricas *stats, int ordem) {
    // Aloca o heap com tamanho MAX_MEMORIA
    HeapNode *heap = (HeapNode *)malloc(MAX_MEMORIA * sizeof(HeapNode));
    if (!heap) return 0; // Retorna 0 se falhar a alocação
    
    int num_ciclos = 0;       // Contador de corridas geradas
    int heap_size = 0;        // Tamanho atual do heap
    int prox_registro = 0;    // Próximo registro a ser lido
    int ciclo_atual = 0;      // Corrida atual
    *tam_fita1 = 0;           // Inicializa tamanho da fita1
    *tam_fita2 = 0;           // Inicializa tamanho da fita2
    
    clock_t inicio, fim;      // Variáveis para medir tempo
    iniciar_tempo(&inicio);   // Inicia a contagem de tempo
    
    // Inicializar o heap com os primeiros MAX_MEMORIA registros
    for (int i = 0; i < MAX_MEMORIA && prox_registro < quantidade; i++) {
        heap[i].nota = registros[prox_registro].nota;
        heap[i].posicao = prox_registro;
        heap[i].ciclo = 0;     // Todos começam na corrida 0
        prox_registro++;
        heap_size++;
        stats->leituras_pre++; // Incrementa contador de leituras
    }
    
    // Construir o heap inicial
    construir_heap(heap, heap_size, ordem);
    
    // Enquanto houver elementos no heap
    while (heap_size > 0) {
        // O elemento mais adequado está na raiz do heap
        NotaPosicao saida;
        saida.nota = heap[0].nota;       // Extrai a nota
        saida.posicao = heap[0].posicao; // Extrai a posição original
        
        // Determinar em qual fita colocar a saída com base na corrida atual
        // Alterna entre as fitas (fita1 para ciclos pares, fita2 para ciclos ímpares)
        if (ciclo_atual % 2 == 0) {
            fita1[(*tam_fita1)++] = saida; // Adiciona à fita1 e incrementa seu tamanho
        } else {
            fita2[(*tam_fita2)++] = saida; // Adiciona à fita2 e incrementa seu tamanho
        }
        stats->escritas_pre++; // Incrementa contador de escritas
        
        // Se ainda houver registros a serem processados
        if (prox_registro < quantidade) {
            // Substitui o elemento removido pelo próximo do input
            float prox_nota = registros[prox_registro].nota;
            
            // Verifica se o próximo registro pode continuar na mesma corrida
            // Baseado na ordem (ascendente ou descendente)
            int pode_continuar = 0;
            if (ordem == ORDEM_ASCENDENTE) {
                // Em ordem ascendente, o próximo deve ser >= ao atual para continuar
                pode_continuar = (prox_nota >= saida.nota);
            } else { // ORDEM_DESCENDENTE
                // Em ordem descendente, o próximo deve ser <= ao atual para continuar
                pode_continuar = (prox_nota <= saida.nota);
            }
            
            if (pode_continuar) {
                // O próximo registro pode continuar na mesma corrida
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
                    ciclo_atual++;    // Avança para a próxima corrida
                    num_ciclos++;     // Incrementa o contador de corridas
                }
            }
            
            prox_registro++;          // Avança para o próximo registro de entrada
            stats->leituras_pre++;    // Incrementa contador de leituras
            
            // Restaurar a propriedade do heap após a substituição na raiz
            descer_no_heap(heap, 0, heap_size, ordem);
            stats->comparacoes_pre += heap_size * 2; // Aproximação do número de comparações
        } else {
            // Não há mais registros de entrada, remove-se o elemento do heap
            heap[0] = heap[heap_size - 1]; // Move o último elemento para a raiz
            heap_size--;                   // Reduz o tamanho do heap
            
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
                ciclo_atual++;    // Avança para a próxima corrida
                num_ciclos++;     // Incrementa o contador de corridas
            }
        }
    }
    
    // Incrementa num_ciclos para contar a última corrida
    num_ciclos++;
    
    free(heap);  // Libera a memória alocada para o heap
    finalizar_tempo(&inicio, &fim, &stats->tempo_execucao_pre); // Finaliza medição de tempo
    
    return num_ciclos; // Retorna o número total de corridas geradas
}

// Função para intercalar duas corridas
// Combina elementos de duas corridas em ordem mantendo a ordenação
void intercalar_corridas(NotaPosicao *resultado, NotaPosicao *fita1, int *idx1, int fim1, 
                        NotaPosicao *fita2, int *idx2, int fim2, int *pos_resultado, 
                        Metricas *stats, int ordem) {
    // Enquanto houver elementos em ambas as corridas
    while (*idx1 <= fim1 && *idx2 <= fim2) {
        stats->comparacoes_pos++; // Incrementa contador de comparações
        
        int selecionar_fita1 = 0;
        if (ordem == ORDEM_ASCENDENTE) {
            // Em ordem ascendente, seleciona o menor entre os dois
            selecionar_fita1 = (fita1[*idx1].nota <= fita2[*idx2].nota);
        } else { // ORDEM_DESCENDENTE
            // Em ordem descendente, seleciona o maior entre os dois
            selecionar_fita1 = (fita1[*idx1].nota >= fita2[*idx2].nota);
        }
        
        if (selecionar_fita1) {
            // Copia o elemento da fita1 para o resultado
            resultado[(*pos_resultado)++] = fita1[(*idx1)++];
        } else {
            // Copia o elemento da fita2 para o resultado
            resultado[(*pos_resultado)++] = fita2[(*idx2)++];
        }
        stats->leituras_pos++; // Incrementa contador de leituras
    }
    
    // Copia os elementos restantes da primeira fita, se houver
    while (*idx1 <= fim1) {
        resultado[(*pos_resultado)++] = fita1[(*idx1)++];
        stats->leituras_pos++; // Incrementa contador de leituras
    }
    
    // Copia os elementos restantes da segunda fita, se houver
    while (*idx2 <= fim2) {
        resultado[(*pos_resultado)++] = fita2[(*idx2)++];
        stats->leituras_pos++; // Incrementa contador de leituras
    }
}

// Função principal de intercalação balanceada com seleção por substituição
// Implementa o algoritmo completo de ordenação externa
void intercalacao_balanceada_2f(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats, int ordem, int imprime) {
    Registro *registros = NULL;
    clock_t inicio, fim;
    iniciar_tempo(&inicio); // Inicia a contagem de tempo
    
    // Lê os registros do arquivo binário
    ler_binario("./data/registros.bin", &registros, quantidade);
    if (!registros) {
        printf("Erro ao ler registros.\n");
        return;
    }
    
    // Aloca memória para as "fitas" (arrays que simulam fitas magnéticas)
    NotaPosicao *fita1 = malloc(quantidade * sizeof(NotaPosicao));
    NotaPosicao *fita2 = malloc(quantidade * sizeof(NotaPosicao));
    NotaPosicao *resultado = malloc(quantidade * sizeof(NotaPosicao));
    int tam_fita1, tam_fita2;
    
    // Verifica se as alocações foram bem-sucedidas
    if (!fita1 || !fita2 || !resultado) {
        printf("Erro ao alocar memória para fitas.\n");
        free(registros);
        if (fita1) free(fita1);
        if (fita2) free(fita2);
        if (resultado) free(resultado);
        return;
    }
    
    // Gera as corridas iniciais usando seleção por substituição
    int num_ciclos = selecao_por_substituicao(registros, quantidade, fita1, fita2, 
                                             &tam_fita1, &tam_fita2, stats, ordem);
    
    // Aloca vetor para armazenar posições ordenadas (resultado final)
    long *posicoes_ordenadas = malloc(quantidade * sizeof(long));
    if (!posicoes_ordenadas) {
        printf("Erro ao alocar memória para posições ordenadas.\n");
        free(registros);
        free(fita1);
        free(fita2);
        free(resultado);
        return;
    }
    
    // Inicia a fase de intercalação
    iniciar_tempo(&inicio);
    
    // Se apenas uma corrida foi gerada, o resultado já está ordenado
    if (num_ciclos <= 1) {
        if (tam_fita1 > 0) {
            // A corrida está na fita1, copia as posições
            for (int i = 0; i < tam_fita1; i++) {
                posicoes_ordenadas[i] = fita1[i].posicao;
                stats->leituras_pos++;
            }
        } else if (tam_fita2 > 0) {
            // A corrida está na fita2, copia as posições
            for (int i = 0; i < tam_fita2; i++) {
                posicoes_ordenadas[i] = fita2[i].posicao;
                stats->leituras_pos++;
            }
        }
    } else {
        // Múltiplas corridas - necessário intercalá-las
        // Cria arrays para rastrear informações de corridas explicitamente
        int *ciclos_fita1 = malloc((num_ciclos + 1) * sizeof(int));
        int *ciclos_fita2 = malloc((num_ciclos + 1) * sizeof(int));
        int num_ciclos_fita1 = 0;
        int num_ciclos_fita2 = 0;
        
        // Inicializa os limites das corridas - distribui corridas uniformemente
        for (int i = 0; i < num_ciclos; i++) {
            if (i % 2 == 0 && tam_fita1 > 0) {
                // Corridas pares vão para a fita1
                if (num_ciclos_fita1 == 0) {
                    ciclos_fita1[num_ciclos_fita1++] = 0; // Início da primeira corrida
                }
                
                // Encontra o fim desta corrida verificando quebras de ordem
                int ciclo_fim = tam_fita1 - 1;
                for (int j = ciclos_fita1[num_ciclos_fita1-1]; j < tam_fita1 - 1; j++) {
                    // Quebra de ordem dependendo da ordem ascendente/descendente
                    if ((ordem == ORDEM_ASCENDENTE && fita1[j].nota > fita1[j+1].nota) ||
                        (ordem == ORDEM_DESCENDENTE && fita1[j].nota < fita1[j+1].nota)) {
                        ciclo_fim = j;
                        break;
                    }
                }
                ciclos_fita1[num_ciclos_fita1++] = ciclo_fim + 1; // Início da próxima corrida
            } else if (tam_fita2 > 0) {
                // Corridas ímpares vão para a fita2
                if (num_ciclos_fita2 == 0) {
                    ciclos_fita2[num_ciclos_fita2++] = 0; // Início da primeira corrida
                }
                
                // Encontra o fim desta corrida
                int ciclo_fim = tam_fita2 - 1;
                for (int j = ciclos_fita2[num_ciclos_fita2-1]; j < tam_fita2 - 1; j++) {
                    // Quebra de ordem dependendo da ordem ascendente/descendente
                    if ((ordem == ORDEM_ASCENDENTE && fita2[j].nota > fita2[j+1].nota) ||
                        (ordem == ORDEM_DESCENDENTE && fita2[j].nota < fita2[j+1].nota)) {
                        ciclo_fim = j;
                        break;
                    }
                }
                ciclos_fita2[num_ciclos_fita2++] = ciclo_fim + 1; // Início da próxima corrida
            }
        }
        
        // Corrige os limites da última corrida
        if (num_ciclos_fita1 > 0) {
            ciclos_fita1[num_ciclos_fita1] = tam_fita1;
        }
        if (num_ciclos_fita2 > 0) {
            ciclos_fita2[num_ciclos_fita2] = tam_fita2;
        }
        
        // Intercala até que reste apenas uma corrida
        while (num_ciclos_fita1 + num_ciclos_fita2 > 1) {
            int pos_resultado = 0;
            int next_fita1 = 0;
            int next_fita2 = 0;
            
            // Intercala pares de corridas
            while (next_fita1 < num_ciclos_fita1 && next_fita2 < num_ciclos_fita2) {
                // Obtém os limites das corridas
                int inicio_fita1 = ciclos_fita1[next_fita1];
                int fim_fita1 = ciclos_fita1[next_fita1 + 1] - 1;
                int inicio_fita2 = ciclos_fita2[next_fita2];
                int fim_fita2 = ciclos_fita2[next_fita2 + 1] - 1;
                
                // Intercala estas duas corridas
                int idx1 = inicio_fita1;
                int idx2 = inicio_fita2;
                intercalar_corridas(resultado, fita1, &idx1, fim_fita1, 
                                   fita2, &idx2, fim_fita2, 
                                   &pos_resultado, stats, ordem);
                
                next_fita1++;
                next_fita2++;
            }
            
            // Trata corridas remanescentes da fita1
            while (next_fita1 < num_ciclos_fita1) {
                int inicio = ciclos_fita1[next_fita1];
                int fim = ciclos_fita1[next_fita1 + 1] - 1;
                
                // Copia a corrida diretamente para o resultado
                for (int i = inicio; i <= fim; i++) {
                    resultado[pos_resultado++] = fita1[i];
                    stats->leituras_pos++;
                    stats->escritas_pos++;
                }
                
                next_fita1++;
            }
            
            // Trata corridas remanescentes da fita2
            while (next_fita2 < num_ciclos_fita2) {
                int inicio = ciclos_fita2[next_fita2];
                int fim = ciclos_fita2[next_fita2 + 1] - 1;
                
                // Copia a corrida diretamente para o resultado
                for (int i = inicio; i <= fim; i++) {
                    resultado[pos_resultado++] = fita2[i];
                    stats->leituras_pos++;
                    stats->escritas_pos++;
                }
                
                next_fita2++;
            }
            
            // Prepara para a próxima iteração - distribui corridas de volta para as fitas
            tam_fita1 = 0;
            tam_fita2 = 0;
            num_ciclos_fita1 = 0;
            num_ciclos_fita2 = 0;
            
            // Reinicia os rastreadores de corridas
            ciclos_fita1[0] = 0;
            ciclos_fita2[0] = 0;
            
            // Vasculha o resultado para encontrar corridas e distribuí-las
            int ciclo_start = 0;
            for (int i = 1; i < pos_resultado; i++) {
                // Detecta quando ocorre uma quebra na ordem
                if ((ordem == ORDEM_ASCENDENTE && resultado[i-1].nota > resultado[i].nota) ||
                    (ordem == ORDEM_DESCENDENTE && resultado[i-1].nota < resultado[i].nota)) {
                    // Fim de uma corrida detectada
                    if (num_ciclos_fita1 <= num_ciclos_fita2) {
                        // Copia para a fita1 (balanceamento)
                        for (int j = ciclo_start; j < i; j++) {
                            fita1[tam_fita1++] = resultado[j];
                        }
                        ciclos_fita1[num_ciclos_fita1 + 1] = tam_fita1;
                        num_ciclos_fita1++;
                    } else {
                        // Copia para a fita2 (balanceamento)
                        for (int j = ciclo_start; j < i; j++) {
                            fita2[tam_fita2++] = resultado[j];
                        }
                        ciclos_fita2[num_ciclos_fita2 + 1] = tam_fita2;
                        num_ciclos_fita2++;
                    }
                    ciclo_start = i;
                }
            }
            
            // Trata a última corrida
            if (ciclo_start < pos_resultado) {
                if (num_ciclos_fita1 <= num_ciclos_fita2) {
                    // Copia para a fita1
                    for (int j = ciclo_start; j < pos_resultado; j++) {
                        fita1[tam_fita1++] = resultado[j];
                    }
                    ciclos_fita1[num_ciclos_fita1 + 1] = tam_fita1;
                    num_ciclos_fita1++;
                } else {
                    // Copia para a fita2
                    for (int j = ciclo_start; j < pos_resultado; j++) {
                        fita2[tam_fita2++] = resultado[j];
                    }
                    ciclos_fita2[num_ciclos_fita2 + 1] = tam_fita2;
                    num_ciclos_fita2++;
                }
            }
        }
        
        // O resultado final está em uma das fitas
        if (num_ciclos_fita1 == 1) {
            // Se a fita1 tem uma corrida, ela contém o resultado final
            for (int i = 0; i < tam_fita1; i++) {
                posicoes_ordenadas[i] = fita1[i].posicao;
            }
        } else if (num_ciclos_fita2 == 1) {
            // Se a fita2 tem uma corrida, ela contém o resultado final
            for (int i = 0; i < tam_fita2; i++) {
                posicoes_ordenadas[i] = fita2[i].posicao;
            }
        }
        
        // Libera a memória alocada para os arrays de corridas
        free(ciclos_fita1);
        free(ciclos_fita2);
    }
    
    // Finaliza a medição do tempo de execução
    finalizar_tempo(&inicio, &fim, &stats->tempo_execucao_pos);
    
    // Registra as métricas de desempenho
    const char* ordem_str = (ordem == ORDEM_ASCENDENTE) ? "Ascendente" : "Descendente";
    char nome_algoritmo[100];
    sprintf(nome_algoritmo, "Intercalacao 2f - Selecao Substituicao (%s)", ordem_str);
    log_metricas(nome_algoritmo, quantidade, situacao == 1 ? "1" : situacao == 2 ? "2" : "3", *stats);
    
    // Imprime os registros ordenados, se solicitado
    if (imprime == 1) {
      printf("\nRegistros ordenados por nota (ordem %s):\n", ordem_str);
      for (int i = 0; i < quantidade; i++) {
          print_registro(&registros[posicoes_ordenadas[i]]);
      } 
    }
    
    // Libera a memória alocada
    free(posicoes_ordenadas);
    free(fita1);
    free(fita2);
    free(resultado);
    free(registros);
}

// Wrapper para a versão ascendente (compatibilidade com código existente)
// Chama a função genérica com parâmetro ORDEM_ASCENDENTE
void intercalacao_balanceada_2f_ascendente(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats, int imprime) {
    intercalacao_balanceada_2f(nome_arquivo, quantidade, situacao, stats, ORDEM_ASCENDENTE, imprime);
}

// Wrapper para a versão descendente
// Chama a função genérica com parâmetro ORDEM_DESCENDENTE
void intercalacao_balanceada_2f_descendente(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats, int imprime) {
    intercalacao_balanceada_2f(nome_arquivo, quantidade, situacao, stats, ORDEM_DESCENDENTE, imprime);
}