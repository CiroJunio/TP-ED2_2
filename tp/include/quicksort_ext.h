#ifndef QUICK_SORT_EXT_H
#define QUICK_SORT_EXT_H

#include "registro.h"
#include "../include/utils.h"
#include "../include/registro.h"
#include "../include/leitura.h"

// Função para trocar dois registros no vetor
void troca(float *a, float *b, Metricas* stats);


void particionar_arquivo(char *arquivo_entrada, char *arquivo_menores, char *arquivo_maiores, float pivo, Metricas* stats);
// Função de particionamento ascendente
int contar_registros(char *arquivo);
float selecionar_pivo(char *arquivo, int situacao, Metricas* stats);

void mesclar_arquivos(char *arquivo_saida, char *arquivo1, char *arquivo2, int situacao, Metricas* stats);
void quicksort_externo_recursivo(char *arquivo, int situacao, Metricas* stats);
void quicksort_externo(char *arquivo, int quantidade, int situacao, int imprime);



#endif // QUICK_SORT_EXT_H
