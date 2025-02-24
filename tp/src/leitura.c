#include "../include/leitura.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> // Para a função isspace

// Função para remover espaços em branco no início e no final de uma string
void trim_string(char *str) {
    int inicio = 0;
    int fim = strlen(str) - 1;

    // Remove espaços no início
    while (isspace((unsigned char)str[inicio])) {
        inicio++;
    }

    // Remove espaços no final
    while (fim >= inicio && isspace((unsigned char)str[fim])) {
        fim--;
    }

    // Move os caracteres para o início da string
    int i;
    for (i = 0; i <= fim - inicio; i++) {
        str[i] = str[inicio + i];
    }

    // Adiciona o terminador nulo
    str[i] = '\0';
}

// Função para ler o arquivo PROVAO.TXT e armazenar os dados em um vetor de Registro
void ler_provao(const char *nome_arquivo, Registro **registros, int quantidade, int situacao) {
    FILE *arquivo = abrir_arquivo(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    *registros = (Registro*)malloc(quantidade * sizeof(Registro));
    if (!(*registros)) {
        perror("Erro ao alocar memória");
        fechar_arquivo(arquivo);
        exit(EXIT_FAILURE);
    }
    memset(*registros, 0, quantidade * sizeof(Registro));

    char linha[100];
    int i = 0; // Índice do vetor de registros
    while (i < quantidade && fgets(linha, sizeof(linha), arquivo)) {
        // Processamento da linha para preencher a estrutura Registro
        char inscricao_str[9], nota_str[6], cidade[51], curso[31];

        strncpy(inscricao_str, linha, 8);
        inscricao_str[8] = '\0';
        trim_string(inscricao_str);
        long inscricao = atol(inscricao_str);

        strncpy(nota_str, linha + 9, 5);
        nota_str[5] = '\0';
        trim_string(nota_str);
        float nota = atof(nota_str);

        if (inscricao == 0) continue; // Ignora inscrições inválidas e continua lendo

        (*registros)[i].id = inscricao;
        (*registros)[i].nota = nota;

        strncpy((*registros)[i].estado, linha + 15, 2);
        (*registros)[i].estado[2] = '\0';

        strncpy(cidade, linha + 18, 50);
        cidade[50] = '\0';
        trim_string(cidade);
        strcpy((*registros)[i].cidade, cidade);

        strncpy(curso, linha + 69, 30);
        curso[30] = '\0';
        trim_string(curso);
        strcpy((*registros)[i].curso, curso);

        i++; // Avança apenas quando um registro válido é armazenado
    }

    fechar_arquivo(arquivo);
}




// Função para abrir um arquivo
FILE *abrir_arquivo(const char *nome, const char *modo) {
    FILE *arquivo = fopen(nome, modo);
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    return arquivo;
}

// Função para fechar um arquivo
void fechar_arquivo(FILE *arquivo) {
    if (arquivo) {
        fclose(arquivo);
    }
}