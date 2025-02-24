#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

// Função para iniciar a contagem de tempo
void iniciar_tempo(clock_t *inicio) {
    *inicio = clock();
}

// Função para finalizar a contagem de tempo e calcular o tempo de execução
void finalizar_tempo(clock_t *inicio, clock_t *fim, double *tempo_execucao) {
    *fim = clock();
    *tempo_execucao = ((double)(*fim - *inicio)) / CLOCKS_PER_SEC;
}

// Função para logar as métricas de pré e pós-processamento
void log_metricas(const char *metodo, int quantidade, const char *situacao, Metricas m) {
    printf("\nMétricas para o método %s com %d registros na situação %s:\n", metodo, quantidade, situacao);
    
    printf("\nMétricas de Pré-processamento:\n");
    printf("Leituras: %d\n", m.leituras_pre);
    printf("Escritas: %d\n", m.escritas_pre);
    printf("Comparações: %d\n", m.comparacoes_pre);
    printf("Tempo de execução: %.6f segundos\n", m.tempo_execucao_pre);
    
    printf("\nMétricas de Pós-processamento:\n");
    printf("Leituras: %d\n", m.leituras_pos);
    printf("Escritas: %d\n", m.escritas_pos);
    printf("Comparações: %d\n", m.comparacoes_pos);
    printf("Tempo de execução: %.6f segundos\n", m.tempo_execucao_pos);
}
