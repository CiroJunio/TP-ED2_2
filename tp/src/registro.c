#include "../include/registro.h"
#include <stdio.h>
#include <stdlib.h>

void print_registro(const Registro *r) {
  printf("Inscricao: %08ld, Nota: %5.1f, Estado: %2s, Cidade: %-50s, Curso: %-30s\n",
         r->id, r->nota, r->estado, r->cidade, r->curso);
}
