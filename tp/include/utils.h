#ifndef UTILS_H
#define UTILS_H

#include <time.h>

typedef struct {
    int leituras_pre;
    int escritas_pre;
    int comparacoes_pre;
    double tempo_execucao_pre;

    int leituras_pos;
    int escritas_pos;
    int comparacoes_pos;
    double tempo_execucao_pos;
} Metricas;

void iniciar_tempo(clock_t *inicio);
void finalizar_tempo(clock_t *inicio, clock_t *fim, double *tempo_execucao);
void log_metricas(const char *metodo, int quantidade, const char *situacao, Metricas m);

#endif // UTILS_H
