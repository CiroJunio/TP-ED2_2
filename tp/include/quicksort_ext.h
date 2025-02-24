#ifndef QUICKSORT_EXT_H
#define QUICKSORT_EXT_H

#include "registro.h"
#include "leitura.h"
#include "utils.h"

#define MAX_REGISTROS_QS 10  // Correto, pois o quicksort externo usa 10 registros na memória

void quicksort_externo(const char *arquivo, int quantidade, int situacao, Metricas *m);

#endif // QUICKSORT_EXT_H
