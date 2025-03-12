#ifndef INTERCALACAO2F_H
#define INTERCALACAO2F_H

#include "registro.h"
#include "utils.h"

typedef struct {
    float nota;    // Nota do registro
    long posicao;  // Posição original no vetor de registros
} NotaPosicao;

void intercalacao_balanceada_2f_ascendente(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats);
void intercalacao_balanceada_2f_descendente(const char *nome_arquivo, int quantidade, int situacao, Metricas *stats);
#endif // INTERCALACAO2F_H