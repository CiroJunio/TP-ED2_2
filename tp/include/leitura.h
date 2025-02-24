#ifndef LEITURA_H
#define LEITURA_H

#include <stdio.h>
#include "registro.h"

FILE *abrir_arquivo(const char *nome, const char *modo);
void fechar_arquivo(FILE *arquivo);
void trim_string(char *str);

// Nova função para ler o arquivo PROVAO.TXT
void ler_provao(const char *nome_arquivo, Registro **registros, int quantidade, int situacao);

void ler_binario(const char *nome_binario, 
  Registro **registros, 
  int quantidade);

#endif // LEITURA_H