#ifndef REGISTRO_H
#define REGISTRO_H

#define TAM_ESTADO 3
#define TAM_CIDADE 51
#define TAM_CURSO 31

typedef struct {
    long id;
    float nota;
    char estado[TAM_ESTADO];
    char cidade[TAM_CIDADE];
    char curso[TAM_CURSO];
} Registro;

void print_registro(const Registro *r);

#endif // REGISTRO_H
