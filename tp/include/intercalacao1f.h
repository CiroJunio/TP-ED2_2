#ifndef INTERCALACAO1F_H
#define INTERCALACAO1F_H

#include "registro.h"
#include "leitura.h"

#define MAX_REGISTROS_1F 10  // Correção para 10 registros na memória
#define NUM_FITAS_1F 20      // 10 fitas de entrada + 10 fitas de saída

void intercalacao_balanceada_1f(const char *arquivo, int n, int situacao);

#endif // INTERCALACAO1F_H
