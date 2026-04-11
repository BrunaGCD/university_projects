# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# |         ~~ DUPLA ~~         |
# |                             |
# | Bruna Gonçalves Corte David |
# | RA: 10425696                |  Atividade - Lab 02 - Batalha de Palavras em Rede
# |                             |
# | Júlia Andrade               |
# | RA: 10428513                |
# |                             |
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

# ============================================================
# Makefile — Batalha de Palavras em Rede
#
# Uso:
#   make          → compila servidor e cliente
#   make clean    → remove binários
#   make help     → mostra esta ajuda
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11

SERVIDOR = servidor
CLIENTE  = cliente

ALL = $(SERVIDOR) $(CLIENTE)

.PHONY: all clean help

all: $(ALL)
	@echo ""
	@echo "✅ Jogo compilado com sucesso!"
	@echo ""
	@echo "Para testar:"
	@echo "  Terminal 1:  ./servidor"
	@echo "  Terminal 2:  ./cliente"
	@echo "  Terminal 3:  ./cliente"

$(SERVIDOR): servidor.c jogo.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

$(CLIENTE): cliente.c jogo.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(ALL)
	@echo "🧹 Binários removidos."

help:
	@echo "Alvos disponíveis:"
	@echo "  make          Compila servidor e cliente"
	@echo "  make clean    Remove binários"
	@echo "  make help     Mostra esta ajuda"