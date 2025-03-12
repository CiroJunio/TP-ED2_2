#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/intercalacao1f.h"
#include "../include/intercalacao2f.h"
#include "../include/quicksort_ext.h"
#include "../include/utils.h"
#include "../include/registro.h"
#include "../include/leitura.h"

#define MAX_SITUACAO 20

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        printf("Uso: ordena <metodo> <quantidade> <situacao> [-P]\n");
        return 1;
    }

    int metodo;
    int quantidade;
    int situacao_int;
    char situacao[MAX_SITUACAO];
    int imprimir = 0;
    int imprimir_aqui = 0;

    // Leitura dos parâmetros
    metodo = atoi(argv[1]);  // Modificado para ser um número inteiro de 1 a 3
    quantidade = atoi(argv[2]);
    situacao_int = atoi(argv[3]);

    // Convertendo a situação para string
    switch (situacao_int) {
        case 1:
            strcpy(situacao, "1");
            break;
        case 2:
            strcpy(situacao, "2");
            break;
        case 3:
            strcpy(situacao, "3");
            break;
        default:
            printf("Situacao inválida. Use 1, 2 ou 3.\n");
            return 1;
    }

    // Verifica se o argumento opcional -P foi passado
    if (argc == 5 && strcmp(argv[4], "-P") == 0) {
        imprimir = 1;
    }

    Registro *registros = NULL;
    //ler_provao("./data/PROVAO.TXT", &registros, quantidade, situacao_int);
    ler_binario("./data/registros.bin", &registros, quantidade);

    Metricas stats = {0, 0, 0, 0.0, 0, 0, 0, 0.0};

    // Usando switch para selecionar o método de ordenação
    switch (metodo) {
        case 1:
            intercalacao_balanceada_1f(argv[2], quantidade, situacao_int);
            break;
        case 2:
            if (situacao_int == 1) {
                intercalacao_balanceada_2f_ascendente(argv[2], quantidade, situacao_int, &stats, imprimir);
                imprimir_aqui = 1;
            }
            else {
                intercalacao_balanceada_2f_descendente(argv[2], quantidade, situacao_int, &stats, imprimir);
                imprimir_aqui = 1;
            }
            break;
        case 3:
            quicksort_externo(argv[2], quantidade, situacao_int, &stats);
            break;
        default:
            printf("Metodo de ordenacao desconhecido.\n");
            return 1;
    }
    if (imprimir == 1 && imprimir_aqui == 0) {
        for (int i = 0; i < quantidade; i++) {
            print_registro(&registros[i]);
        }
    }
    return 0;
}
