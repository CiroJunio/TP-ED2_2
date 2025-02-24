#ifndef INTERCALACAO2F_H
#define INTERCALACAO2F_H

#include "registro.h"
#include "leitura.h"
#include "utils.h"

#define MAX_REGISTROS_2F 19  // Correção para 19 registros na memória
#define NUM_FITAS_2F 20      // 19 fitas de entrada + 1 fita de saída

void intercalacao_balanceada_2f(const char *arquivo, int quantidade, int situacao, Metricas *m);

#endif // INTERCALACAO2F_H
