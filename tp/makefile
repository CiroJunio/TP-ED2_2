# Diretórios
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
DATA_DIR = data

# Arquivo de saída
OUTPUT = ordena

# Flags de compilação
CC = gcc
CFLAGS = -Wall -g -I$(INC_DIR)  # Incluir diretório de cabeçalhos

# Lista de arquivos fonte
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Gerar lista de objetos a partir dos fontes
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Alvo padrão
all: $(OUTPUT)

# Compilar o programa
$(OUTPUT): $(OBJECTS)
	$(CC) -o $@ $^

# Compilar os arquivos objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Criar diretório de objetos, se não existir
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar os arquivos objeto, binários e dados
clean:
	@echo "Cleaning files..."
	rm -rf $(OBJ_DIR) $(OUTPUT) $(DATA_DIR)/* $(DATA_DIR)/.*  # Apagar conteúdo da pasta data/

# Recompilar tudo
rebuild: clean all

.PHONY: all clean rebuild
