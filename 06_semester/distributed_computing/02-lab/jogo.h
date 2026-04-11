/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade - Lab 02 - Batalha de Palavras em Rede
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>

// ---- Lógica do Jogo ----

//Sorteia e retorna uma letra maiúscula do alfabeto (A-Z)
char sortear_letra(void);
// Retorna true se a palavra for válida, false caso contrário
bool validar_palavra(const char *palavra, char letra_rodada);

// ---- Auxiliares de String ----

// Remove caracteres de quebra de linha do final da string
void remover_quebra_linha(char *string);
// Lê a entrada do usuário com um tempo limite
int receber_com_timeout(char *buffer, int tamanho_maximo, int segundos);

#endif